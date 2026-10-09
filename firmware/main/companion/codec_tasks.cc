#include "codec_tasks.h"
#include <esp_opus_enc.h>
#include <esp_opus_dec.h>
#include <esp_timer.h>
#include <algorithm>
#include <array>
namespace companion {
static uint64_t now_ms(){return esp_timer_get_time()/1000;}
CodecTasks::~CodecTasks(){configASSERT(done_.load());CloseCodecs();}
bool CodecTasks::OpenCodecs(){
    CloseCodecs();
    esp_opus_enc_config_t enc={};enc.sample_rate=16000;enc.channel=ESP_AUDIO_MONO;enc.bits_per_sample=ESP_AUDIO_BIT16;
    enc.bitrate=24000;enc.frame_duration=ESP_OPUS_ENC_FRAME_DURATION_60_MS;enc.application_mode=ESP_OPUS_ENC_APPLICATION_VOIP;
    enc.complexity=0;enc.enable_dtx=false;enc.enable_vbr=true;
    if(esp_opus_enc_open(&enc,sizeof(enc),&encoder_)!=ESP_AUDIO_ERR_OK)return false;
    esp_opus_dec_cfg_t dec={};dec.sample_rate=24000;dec.channel=ESP_AUDIO_MONO;dec.frame_duration=ESP_OPUS_DEC_FRAME_DURATION_60_MS;
    if(esp_opus_dec_open(&dec,sizeof(dec),&decoder_)!=ESP_AUDIO_ERR_OK)return false;
    esp_ae_rate_cvt_cfg_t rate={};rate.src_rate=24000;rate.dest_rate=16000;rate.channel=1;rate.bits_per_sample=16;
    rate.complexity=2;rate.perf_type=ESP_AE_RATE_CVT_PERF_TYPE_SPEED;
    if(esp_ae_rate_cvt_open(&rate,&resampler_)!=ESP_AE_ERR_OK)return false;
    pending_.clear();pending_.reserve(1440);mic_sequence_=0;return true;
}
void CodecTasks::CloseCodecs(){
    if(encoder_){esp_opus_enc_close(encoder_);encoder_=nullptr;}
    if(decoder_){esp_opus_dec_close(decoder_);decoder_=nullptr;}
    if(resampler_){esp_ae_rate_cvt_close(resampler_);resampler_=nullptr;}
}
bool CodecTasks::Start(AudioCodec* codec,Send send,Stopped stopped){
    if(!done_ || !codec || !send || !stopped || codec->input_enabled() || codec->output_enabled() || codec->input_sample_rate()!=24000 || codec->output_sample_rate()!=24000 || codec->input_channels()<1 || codec->input_channels()>2)return false;
    codec_=codec;send_=std::move(send);stopped_=std::move(stopped);quit_=false;enabled_=false;done_=false;
    if(xTaskCreate([](void* arg){auto self=static_cast<CodecTasks*>(arg);self->Run();self->done_.store(true);vTaskDelete(nullptr);},"companion_codec",16384,this,4,&task_)!=pdPASS){done_=true;return false;}
    return true;
}
bool CodecTasks::ResetStoppedConnection(){
    if(!done_)return false;
    std::lock_guard<std::mutex> lock(mutex_);session_=AudioSession{};incoming_.clear();epoch_=0;enabled_=false;playing_=false;playback_known_=false;return true;
}
bool CodecTasks::Stop(uint32_t timeout_ms){
    enabled_=false;quit_=true;auto until=now_ms()+std::min<uint32_t>(timeout_ms,5000);
    while(!done_ && now_ms()<until)vTaskDelay(pdMS_TO_TICKS(10));
    return done_;
}
bool CodecTasks::Begin(uint32_t epoch){
    enabled_=false;std::lock_guard<std::mutex> lock(mutex_);
    if(!session_.begin(epoch,now_ms()))return false;
    incoming_.clear();playing_=false;playback_known_=false;epoch_.store(epoch);return true;
}
bool CodecTasks::Acknowledge(uint32_t epoch){std::lock_guard<std::mutex> lock(mutex_);return session_.acknowledge(epoch);}
bool CodecTasks::Consent(uint32_t epoch,bool physical){std::lock_guard<std::mutex> lock(mutex_);auto result=session_.arm(epoch,physical);enabled_=result;return result;}
bool CodecTasks::Heartbeat(uint32_t epoch){std::lock_guard<std::mutex> lock(mutex_);auto result=session_.heartbeat(epoch,now_ms());if(!result)enabled_=false;return result;}
bool CodecTasks::Playing(uint32_t epoch,bool playing){std::lock_guard<std::mutex> lock(mutex_);auto result=session_.playing(epoch,playing);if(result){playing_=playing;playback_known_=true;if(!playing)incoming_.clear();}return result;}
void CodecTasks::Interrupt(uint32_t epoch){
    if(epoch!=epoch_.load())return;
    enabled_=false;playing_=false;playback_known_=false;std::lock_guard<std::mutex> lock(mutex_);session_.interrupt(epoch);incoming_.clear();
}
bool CodecTasks::Current(uint32_t epoch){
    if(!enabled_ || epoch!=epoch_.load() || quit_)return false;
    std::lock_guard<std::mutex> lock(mutex_);auto active=session_.tick(now_ms());if(!active)enabled_=false;return active;
}
bool CodecTasks::Receive(const uint8_t* data,std::size_t bytes){
    WireFrame frame{};if(!decode_wire(data,bytes,frame) || !frame.speaker)return false;
    std::lock_guard<std::mutex> lock(mutex_);
    if(!enabled_ || incoming_.size()>=2 || !session_.packet(frame.epoch,frame.sequence,frame.size,true,now_ms())){enabled_=false;session_.mute();incoming_.clear();return false;}
    incoming_.push_back({frame.epoch,std::vector<uint8_t>(frame.payload,frame.payload+frame.size)});return true;
}
void CodecTasks::Run(){
    uint32_t codec_epoch=0;bool was_active=false;bool failed=false;
    std::vector<int16_t> hardware(480*codec_->input_channels()),mono(480),converted(512),playback(1440),slice(480);
    std::array<uint8_t,opus_max> opus{};std::array<uint8_t,wire_header+opus_max> wire{};
    while(!quit_){
        auto epoch=epoch_.load();
        if(!Current(epoch)){
            codec_->EnableInput(false);codec_->EnableOutput(false);pending_.clear();
            if(was_active){CloseCodecs();codec_epoch=0;stopped_(epoch,false);was_active=false;} // false: physical silence not attested
            vTaskDelay(pdMS_TO_TICKS(10));continue;
        }
        was_active=true;
        // Arming authorizes the session, but does not choose capture vs playback.
        // Await the host's first explicit playing value without touching input.
        if(!playback_known_){
            codec_->EnableInput(false);codec_->EnableOutput(false);pending_.clear();
            vTaskDelay(pdMS_TO_TICKS(10));continue;
        }
        if(!playing_)codec_->EnableOutput(false);
        if(codec_epoch!=epoch){if(!OpenCodecs()){failed=true;break;}codec_epoch=epoch;}
        Packet packet{};
        {std::lock_guard<std::mutex> lock(mutex_);if(!incoming_.empty()){packet=std::move(incoming_.front());incoming_.pop_front();}}
        if(!packet.payload.empty()){
            codec_->EnableInput(false);pending_.clear();
            if(packet.epoch!=epoch || !playing_)continue;
            esp_audio_dec_in_raw_t raw={};raw.buffer=packet.payload.data();raw.len=packet.payload.size();raw.frame_recover=ESP_AUDIO_DEC_RECOVERY_NONE;
            esp_audio_dec_out_frame_t out={};out.buffer=reinterpret_cast<uint8_t*>(playback.data());out.len=playback.size()*2;
            esp_audio_dec_info_t info={};
            if(esp_opus_dec_decode(decoder_,&raw,&out,&info)!=ESP_AUDIO_ERR_OK || out.decoded_size!=2880 || raw.consumed!=raw.len){failed=true;break;}
            if(!playing_ || !Current(epoch))continue;
            codec_->EnableOutput(true);
            for(std::size_t i=0;i<playback.size() && playing_ && Current(epoch);i+=480){std::copy_n(playback.data()+i,480,slice.data());codec_->OutputData(slice);}
            // Keep the device open through a continuous speaking interval,
            // including bounded gaps between Opus packets. Close on direction
            // change, interruption or worker teardown instead of every packet.
            if(!playing_)codec_->EnableOutput(false);
            esp_ae_rate_cvt_reset(resampler_);continue;
        }
        if(playing_){codec_->EnableInput(false);pending_.clear();vTaskDelay(pdMS_TO_TICKS(10));continue;}
        codec_->EnableInput(true);
        if(!codec_->InputData(hardware)){failed=true;break;}
        if(!Current(epoch) || playing_)continue;
        for(std::size_t i=0;i<480;++i)mono[i]=hardware[i*codec_->input_channels()];
        uint32_t size=0;
        if(esp_ae_rate_cvt_get_max_out_sample_num(resampler_,480,&size)!=ESP_AE_ERR_OK || size>converted.size()){failed=true;break;}
        if(esp_ae_rate_cvt_process(resampler_,mono.data(),480,converted.data(),&size)!=ESP_AE_ERR_OK || pending_.size()+size>1440){failed=true;break;}
        pending_.insert(pending_.end(),converted.begin(),converted.begin()+size);
        if(pending_.size()>=960){
            esp_audio_enc_in_frame_t in={};in.buffer=reinterpret_cast<uint8_t*>(pending_.data());in.len=1920;
            esp_audio_enc_out_frame_t out={};out.buffer=opus.data();out.len=opus.size();
            if(esp_opus_enc_process(encoder_,&in,&out)!=ESP_AUDIO_ERR_OK || !out.encoded_bytes || out.encoded_bytes>opus.size()){failed=true;break;}
            pending_.erase(pending_.begin(),pending_.begin()+960);
            bool allowed=false;{std::lock_guard<std::mutex> lock(mutex_);allowed=session_.packet(epoch,mic_sequence_,out.encoded_bytes,false,now_ms());}
            if(!allowed || !Current(epoch))continue;
            auto bytes=encode_wire({epoch,mic_sequence_++,false,opus.data(),out.encoded_bytes},wire.data(),wire.size());
            if(!bytes || !send_(wire.data(),bytes)){failed=true;break;}
        }
    }
    enabled_=false;codec_->EnableInput(false);codec_->EnableOutput(false);CloseCodecs();
    {std::lock_guard<std::mutex> lock(mutex_);incoming_.clear();session_.mute();}
    stopped_(epoch_.load(),false);(void)failed;
}
}
