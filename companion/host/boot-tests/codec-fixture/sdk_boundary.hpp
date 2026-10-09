#pragma once
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <functional>
#include <vector>
namespace codec_fixture {
inline uint64_t clock_ms=100;
inline void (*entry)(void*)=nullptr;
inline void* argument=nullptr;
inline std::function<void()> delay;
inline int decode_result=0;
inline std::size_t decoded_bytes=2880;
inline bool consume_all=true;
inline unsigned decode_calls=0;
}
using TaskHandle_t=void*;
constexpr int pdPASS=1;
#define configASSERT(x) assert(x)
#define pdMS_TO_TICKS(x) (x)
inline int xTaskCreate(void (*entry)(void*),const char*,int,void* arg,int,TaskHandle_t* handle){codec_fixture::entry=entry;codec_fixture::argument=arg;*handle=arg;return pdPASS;}
inline void vTaskDelay(unsigned ms){codec_fixture::clock_ms+=ms;if(codec_fixture::delay)codec_fixture::delay();}
inline void vTaskDelete(void*){}
inline int64_t esp_timer_get_time(){return codec_fixture::clock_ms*1000;}
class AudioCodec {
protected: bool input_=false,output_=false;
public:
 virtual ~AudioCodec()=default;
 bool input_enabled()const{return input_;} bool output_enabled()const{return output_;}
 int input_sample_rate()const{return 24000;} int output_sample_rate()const{return 24000;} int input_channels()const{return 1;}
 virtual void EnableInput(bool b){input_=b;} virtual void EnableOutput(bool b){output_=b;}
 virtual bool InputData(std::vector<int16_t>&)=0;
 virtual void OutputData(std::vector<int16_t>&)=0;
};
constexpr int ESP_AUDIO_MONO=1,ESP_AUDIO_BIT16=16,ESP_OPUS_ENC_FRAME_DURATION_60_MS=60,ESP_OPUS_ENC_APPLICATION_VOIP=1,ESP_OPUS_DEC_FRAME_DURATION_60_MS=60,ESP_AE_RATE_CVT_PERF_TYPE_SPEED=1,ESP_AUDIO_DEC_RECOVERY_NONE=0,ESP_AUDIO_ERR_OK=0,ESP_AE_ERR_OK=0;
struct esp_opus_enc_config_t{int sample_rate,channel,bits_per_sample,bitrate,frame_duration,application_mode,complexity;bool enable_dtx,enable_vbr;};
struct esp_opus_dec_cfg_t{int sample_rate,channel,frame_duration;};
struct esp_ae_rate_cvt_cfg_t{int src_rate,dest_rate,channel,bits_per_sample,complexity,perf_type;};
using esp_ae_rate_cvt_handle_t=void*;
struct esp_audio_dec_in_raw_t{uint8_t* buffer;std::size_t len;int frame_recover;std::size_t consumed;};
struct esp_audio_dec_out_frame_t{uint8_t* buffer;std::size_t len,decoded_size;};
struct esp_audio_dec_info_t{};
struct esp_audio_enc_in_frame_t{uint8_t* buffer;std::size_t len;};
struct esp_audio_enc_out_frame_t{uint8_t* buffer;std::size_t len,encoded_bytes;};
inline int esp_opus_enc_open(const esp_opus_enc_config_t*,std::size_t,void** p){*p=reinterpret_cast<void*>(1);return 0;}
inline int esp_opus_dec_open(const esp_opus_dec_cfg_t*,std::size_t,void** p){*p=reinterpret_cast<void*>(1);return 0;}
inline int esp_ae_rate_cvt_open(const esp_ae_rate_cvt_cfg_t*,void** p){*p=reinterpret_cast<void*>(1);return 0;}
inline void esp_opus_enc_close(void*){} inline void esp_opus_dec_close(void*){} inline void esp_ae_rate_cvt_close(void*){} inline void esp_ae_rate_cvt_reset(void*){}
inline int esp_opus_dec_decode(void*,esp_audio_dec_in_raw_t* in,esp_audio_dec_out_frame_t* out,esp_audio_dec_info_t*){++codec_fixture::decode_calls;in->consumed=codec_fixture::consume_all?in->len:0;out->decoded_size=codec_fixture::decoded_bytes;return codec_fixture::decode_result;}
inline int esp_ae_rate_cvt_get_max_out_sample_num(void*,int,uint32_t* n){*n=0;return 0;}
inline int esp_ae_rate_cvt_process(void*,const int16_t*,int,int16_t*,uint32_t* n){*n=0;return 0;}
inline int esp_opus_enc_process(void*,esp_audio_enc_in_frame_t*,esp_audio_enc_out_frame_t* out){out->encoded_bytes=1;return 0;}
