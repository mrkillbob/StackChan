#include "companion_socket.h"
#include <cJSON.h>
#include <esp_timer.h>
#include <cmath>
#include <set>
namespace companion {
static uint64_t ms(){return esp_timer_get_time()/1000;}
static bool bounded(const std::string& text,std::size_t max){if(text.empty() || text.size()>max)return false;for(auto c:text)if(c<32 || c==127)return false;return true;}
bool CompanionSocket::Text(const std::string& text){std::shared_ptr<BoundedWebSocket> ws;{std::lock_guard<std::mutex> lock(mutex_);ws=socket_;}return ws && ws->IsConnected() && ws->Send(text);}
bool CompanionSocket::SendAudio(const uint8_t* data,std::size_t bytes){
    WireFrame frame{};if(!decode_wire(data,bytes,frame) || frame.speaker || frame.epoch!=epoch_)return false;
    std::shared_ptr<BoundedWebSocket> ws;{std::lock_guard<std::mutex> lock(mutex_);ws=socket_;}return ws && ws->IsConnected() && ws->Send(data,bytes,true);
}
bool CompanionSocket::Connect(NetworkInterface& network,const std::string& uri,const std::string& device,const std::string& credential,const std::string& request){
    if(uri.rfind("wss://",0)!=0 || uri.size()>256 || uri.find_first_of("\r\n@?#")!=std::string::npos || uri.size()<22 || uri.substr(uri.size()-16)!="/companion/audio" || !bounded(device,100) || !bounded(credential,256) || !bounded(request,128))return false;
    Close();if(!tasks_.ResetStoppedConnection())return false;request_id_=request;consent_=false;
    std::shared_ptr<BoundedWebSocket> ws=std::make_shared<BoundedWebSocket>(&network,2);if(!ws)return false;
    ws->SetReceiveBufferSize(4112);ws->SetHeader("Authorization",("Bearer "+credential).c_str());ws->SetHeader("X-Companion-Device",device.c_str());
    ws->OnData([this](const char* data,size_t bytes,bool binary){Data(data,bytes,binary);});
    ws->OnDisconnected([this]{consent_=false;tasks_.Interrupt(epoch_);});
    ws->OnError([this](int){consent_=false;tasks_.Interrupt(epoch_);});
    {std::lock_guard<std::mutex> lock(mutex_);socket_=ws;}
    if(!ws->Connect(uri.c_str())){Close();return false;}
    cJSON* root=cJSON_CreateObject();cJSON_AddStringToObject(root,"type","begin");cJSON_AddStringToObject(root,"request_id",request.c_str());
    char* encoded=cJSON_PrintUnformatted(root);bool ok=encoded && Text(encoded);cJSON_free(encoded);cJSON_Delete(root);return ok;
}
bool CompanionSocket::PhysicalConsent(){
    auto epoch=epoch_.load();if(!epoch)return false;consent_=true;
    return Text("{\"type\":\"consent\",\"epoch\":"+std::to_string(epoch)+",\"physical\":true}");
}
void CompanionSocket::Tick(){if(!epoch_)return;auto now=ms();if(now-last_heartbeat_ms_>=500){last_heartbeat_ms_=now;if(!Text("{\"type\":\"heartbeat\",\"epoch\":"+std::to_string(epoch_.load())+"}"))tasks_.Interrupt(epoch_);}}
void CompanionSocket::Close(){
    consent_=false;tasks_.Interrupt(epoch_);epoch_=0;
    std::shared_ptr<BoundedWebSocket> ws;{std::lock_guard<std::mutex> lock(mutex_);ws=std::move(socket_);}if(ws)ws->Close();
}
void CompanionSocket::Data(const char* data,std::size_t bytes,bool binary){
    if(binary){if(!tasks_.Receive(reinterpret_cast<const uint8_t*>(data),bytes))tasks_.Interrupt(epoch_);return;}
    if(!data || !bytes || bytes>1024){tasks_.Interrupt(epoch_);return;}
    auto root=cJSON_ParseWithLength(data,bytes);if(!root || !cJSON_IsObject(root)){cJSON_Delete(root);tasks_.Interrupt(epoch_);return;}
    std::set<std::string> keys;bool duplicate=false;for(auto item=root->child;item;item=item->next){if(!item->string || !keys.insert(item->string).second)duplicate=true;}
    auto type=cJSON_GetObjectItemCaseSensitive(root,"type"),ep=cJSON_GetObjectItemCaseSensitive(root,"epoch");
    bool valid=!duplicate && cJSON_IsString(type) && cJSON_IsNumber(ep) && ep->valuedouble>=1 && ep->valuedouble<=UINT32_MAX && std::floor(ep->valuedouble)==ep->valuedouble;
    auto epoch=valid?static_cast<uint32_t>(ep->valuedouble):0;
    if(valid && std::string(type->valuestring)=="begin"){
        auto audio=cJSON_GetObjectItemCaseSensitive(root,"audio"),fixture=cJSON_GetObjectItemCaseSensitive(root,"fixture");
        auto number=[audio](const char* key,double expected){auto value=cJSON_GetObjectItemCaseSensitive(audio,key);return cJSON_IsNumber(value) && value->valuedouble==expected;};
        valid=cJSON_IsObject(audio) && number("uplink_rate",16000) && number("downlink_rate",24000) && number("channels",1) && number("frame_ms",60) && cJSON_IsFalse(fixture);
        if(valid && tasks_.Begin(epoch) && tasks_.Acknowledge(epoch)){epoch_=epoch;consent_=false;last_heartbeat_ms_=ms();valid=Text("{\"type\":\"ack\",\"epoch\":"+std::to_string(epoch)+"}");}else valid=false;
    }else if(valid && epoch==epoch_){
        auto kind=std::string(type->valuestring);
        if(kind=="armed")valid=consent_ && tasks_.Consent(epoch,true);
        else if(kind=="heartbeat")valid=tasks_.Heartbeat(epoch);
        else if(kind=="playing"){auto value=cJSON_GetObjectItemCaseSensitive(root,"value");valid=cJSON_IsBool(value) && tasks_.Playing(epoch,cJSON_IsTrue(value));}
        else if(kind=="interrupt"){consent_=false;tasks_.Interrupt(epoch);}
        else if(kind!="acknowledged")valid=false;
    }else valid=false;
    cJSON_Delete(root);if(!valid){consent_=false;tasks_.Interrupt(epoch_);}
}
}
