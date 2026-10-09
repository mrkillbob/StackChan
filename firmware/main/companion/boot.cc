#include "boot.h"
#include "audio_owner.hpp"
#include "usb_owner.hpp"
#include "numeric_usb.hpp"
#include "numeric_pairing.h"
#include "local_tls.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "connect_gate.hpp"
#include <driver/usb_serial_jtag.h>
#include <esp_mac.h>
#include <esp_random.h>
#include <esp_system.h>
#include <esp_timer.h>
#include <cstdio>
#ifndef STACKCHAN_COMPANION_TRUSTED_USB_PROVISIONING
#define STACKCHAN_COMPANION_TRUSTED_USB_PROVISIONING 0
#endif
#include "companion_socket.h"
#include <hal/hal.h>
#include <board.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_netif.h>
#include <nvs.h>
#include <ssid_manager.h>
#include <apps/network_setup/wifi_credentials.hpp>
#include <lvgl.h>
#include <atomic>
#include <algorithm>
#include <cstring>
#include <mutex>
#include <optional>
namespace companion {
namespace {
std::mutex grant_mutex;
std::optional<AuthorizedGrant> pending_grant;
std::atomic<bool> confirm_requested{false},reject_requested{false};
std::atomic<bool> wifi_initialized{false};
std::atomic<bool> pair_requested{false}, connect_requested{false}, listen_requested{false}, mute_requested{false}, connected{false};
// ESP-IDF requires interrupt install/uninstall on the same core. The shell
// task is pinned to CPU0; the helper also enforces that contract at runtime.
struct UsbSdkDriver {
    bool OnOwnerCore() const { return xPortGetCoreID()==0; }
    bool Installed() const { return usb_serial_jtag_is_driver_installed(); }
    bool Install() {
        usb_serial_jtag_driver_config_t config={};config.rx_buffer_size=1024;config.tx_buffer_size=64;
        return usb_serial_jtag_driver_install(&config)==ESP_OK;
    }
    bool Uninstall() { return usb_serial_jtag_driver_uninstall()==ESP_OK; }
    int Read(uint8_t* bytes,size_t capacity) { return usb_serial_jtag_read_bytes(bytes,capacity,0); }
    int WriteAck(const uint8_t* bytes,size_t size) { return usb_serial_jtag_write_bytes(bytes,size,pdMS_TO_TICKS(100)); }
    bool FlushAck() { return usb_serial_jtag_wait_tx_done(pdMS_TO_TICKS(100))==ESP_OK; }
};
class UsbProvisioner {
    UsbSdkDriver driver_;
    const uint32_t boot_nonce_=esp_random();
    UsbProvisioningOwner<UsbSdkDriver> owner_{driver_};
    NumericUsbOwner<UsbSdkDriver> numeric_owner_{driver_};
    bool numeric_=false;
public:
    bool Close() {
        bool closed=numeric_owner_.Close()&&owner_.Close();
        if(!closed) ESP_LOGW("CompanionUSB","USB owner cleanup failed (%d)",static_cast<int>(owner_.LastError()));
        return closed;
    }
    bool Active() const { return numeric_?numeric_owner_.Active():owner_.Active(); }
    bool Numeric()const{return numeric_;}
    uint32_t BootNonce()const{return boot_nonce_?boot_nonce_:1;}
    NumericUsbRead PollNumeric(NumericUsbRequest& request){return numeric_owner_.Poll(esp_timer_get_time()/1000,request);}
    bool Open() {
        if(!STACKCHAN_COMPANION_TRUSTED_USB_PROVISIONING||!Close())return false;
        const char* ca=companion_local_ca_pem();const char* host=companion_local_tls_host();
        numeric_=ca&&*ca&&host&&std::strcmp(host,"192.168.1.75")==0;
        if(numeric_)return numeric_owner_.Open(esp_timer_get_time()/1000,BootNonce(),static_cast<uint32_t>(esp_reset_reason()));
        uint8_t mac[6];if(esp_read_mac(mac,ESP_MAC_WIFI_STA)!=ESP_OK)return false;
        char device[32];std::snprintf(device,sizeof(device),"stackchan-%02x%02x%02x%02x%02x%02x",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
        return owner_.Open(esp_timer_get_time()/1000,device,true,boot_nonce_?boot_nonce_:1,static_cast<uint32_t>(esp_reset_reason()));
    }
    ProvisioningRead Poll(ProvisioningFields& fields) { return owner_.Poll(esp_timer_get_time()/1000,fields); }
    bool CompleteAccepted(bool staged,const std::string& request) { return owner_.CompleteAccepted(staged,request); }
};
void wifi_event(void*, esp_event_base_t base, int32_t id, void*) {
    if(base==IP_EVENT && id==IP_EVENT_STA_GOT_IP) connected=true;
    if(base==WIFI_EVENT && id==WIFI_EVENT_STA_DISCONNECTED) {connected=false;mute_requested=true;}
}
bool StartSavedWifi() {
    if(wifi_initialized.load())return false;
    const auto& saved=SsidManager::GetInstance().GetSsidList();
    if(saved.empty()||!network_setup::Valid({saved.front().ssid,saved.front().password}))return false;
    wifi_config_t wifi={};
    std::memcpy(wifi.sta.ssid,saved.front().ssid.data(),saved.front().ssid.size());
    std::memcpy(wifi.sta.password,saved.front().password.data(),saved.front().password.size());
    if(esp_netif_init()!=ESP_OK)return false;
    auto loop=esp_event_loop_create_default();if(loop!=ESP_OK && loop!=ESP_ERR_INVALID_STATE)return false;
    if(!esp_netif_create_default_wifi_sta())return false;
    wifi_init_config_t config=WIFI_INIT_CONFIG_DEFAULT();config.nvs_enable=false;
    if(esp_wifi_init(&config)!=ESP_OK)return false;
    wifi_initialized=true;
    if(esp_event_handler_register(WIFI_EVENT,ESP_EVENT_ANY_ID,wifi_event,nullptr)!=ESP_OK || esp_event_handler_register(IP_EVENT,IP_EVENT_STA_GOT_IP,wifi_event,nullptr)!=ESP_OK)return false;
    bool ok=esp_wifi_set_storage(WIFI_STORAGE_RAM)==ESP_OK && esp_wifi_set_mode(WIFI_MODE_STA)==ESP_OK && esp_wifi_set_config(WIFI_IF_STA,&wifi)==ESP_OK && esp_wifi_start()==ESP_OK && esp_wifi_connect()==ESP_OK;
    std::memset(&wifi,0,sizeof(wifi));return ok;
}
lv_obj_t* button(lv_obj_t* screen,const char* text,int x,std::atomic<bool>& action) {
    auto b=lv_button_create(screen);lv_obj_set_size(b,140,48);lv_obj_align(b,LV_ALIGN_BOTTOM_MID,x,-12);
    auto label=lv_label_create(b);lv_label_set_text(label,text);lv_obj_center(label);
    lv_obj_add_event_cb(b,[](lv_event_t* e){static_cast<std::atomic<bool>*>(lv_event_get_user_data(e))->store(true);},LV_EVENT_CLICKED,&action);return b;
}
}
bool StageAuthorizedGrant(AuthorizedGrant grant) {
    if((grant.expires_at_ms&&static_cast<uint64_t>(esp_timer_get_time()/1000)>=grant.expires_at_ms) || !grant.host_audio_qualified || grant.credential.empty() || grant.credential.size()>256 || grant.uri.rfind("wss://",0)!=0 || grant.device.empty() || grant.request.empty()){std::fill(grant.credential.begin(),grant.credential.end(),'\0');return false;}
    std::lock_guard<std::mutex> lock(grant_mutex);if(pending_grant){std::fill(grant.credential.begin(),grant.credential.end(),'\0');return false;}
    pending_grant=std::move(grant);return true;
}
namespace {
// Persistent boot-lifetime owner: never destroy callbacks while workers unwind.
struct Runtime {
    UsbProvisioner provisioner;
    NumericPairing numeric_pairing;
    NumericUsbRequest numeric_request;
    uint64_t numeric_until=0;
    bool numeric_connected=false;
    CodecTasks tasks;
    CompanionSocket socket{tasks};
    bool wifi_started=false,grant_connected=false;
    ConnectGate connect_gate;
    lv_obj_t* status=nullptr;
    lv_obj_t *pair_button=nullptr,*listen_button=nullptr,*connect_button=nullptr,*mute_button=nullptr,*confirm_button=nullptr,*reject_button=nullptr;
    const char* message="Mira companion\nMuted - pair a trusted host\nCamera and motion inactive";
};
Runtime& runtime() { static auto* value=new Runtime;return *value; }
}
bool EnsureSavedNetwork() {
    auto& state=runtime();
    if(state.wifi_started)return true;
    state.wifi_started=StartSavedWifi();
    return state.wifi_started;
}
bool IsWifiConnected(){return connected.load();}
bool NetworkWasStarted(){return wifi_initialized.load();}
void OpenView(lv_obj_t* parent) {
    auto& state=runtime();auto& status=state.status;
    pair_requested=false;connect_requested=false;listen_requested=false;mute_requested=false;confirm_requested=false;reject_requested=false;
    {LvglLockGuard lock;auto screen=parent;
     lv_obj_set_style_bg_color(screen,lv_color_hex(0x10232d),0);
     status=lv_label_create(screen);lv_obj_set_width(status,240);lv_obj_set_height(status,54);lv_label_set_long_mode(status,LV_LABEL_LONG_DOT);lv_obj_align(status,LV_ALIGN_TOP_LEFT,8,34);
     lv_obj_set_style_text_color(status,lv_color_hex(0xffffff),0);
     lv_label_set_text(status,"Mira companion\nMuted - pair a trusted host\nCamera and motion inactive");
     auto pair=lv_button_create(screen);state.pair_button=pair;lv_obj_set_size(pair,290,32);lv_obj_align(pair,LV_ALIGN_CENTER,0,-12);
     auto pair_label=lv_label_create(pair);lv_label_set_text(pair_label,"Pair trusted host");lv_obj_center(pair_label);
     lv_obj_add_event_cb(pair,[](lv_event_t*){pair_requested=true;},LV_EVENT_CLICKED,nullptr);
     state.connect_button=button(screen,"Connect",-76,connect_requested);state.mute_button=button(screen,"Mute",76,mute_requested);
     state.confirm_button=button(screen,"Confirm code",-76,confirm_requested);state.reject_button=button(screen,"Reject",76,reject_requested);
     lv_obj_add_flag(state.confirm_button,LV_OBJ_FLAG_HIDDEN);lv_obj_add_flag(state.reject_button,LV_OBJ_FLAG_HIDDEN);
     auto listen=lv_button_create(screen);state.listen_button=listen;lv_obj_set_size(listen,290,48);lv_obj_align(listen,LV_ALIGN_CENTER,0,32);
     auto label=lv_label_create(listen);lv_label_set_text(label,"Allow this conversation");lv_obj_center(label);
     lv_obj_add_event_cb(listen,[](lv_event_t*){listen_requested=true;},LV_EVENT_CLICKED,nullptr);
    }
    state.message="Mira companion\nMuted - pair a trusted host\nCamera and motion inactive";
}
bool CloseView() {
    auto& state=runtime();bool usb_closed=state.provisioner.Close();state.numeric_pairing.Cancel();state.numeric_until=0;state.numeric_request={};state.numeric_connected=false;state.connect_gate.Cancel();
    pair_requested=false;connect_requested=false;listen_requested=false;mute_requested=false;confirm_requested=false;reject_requested=false;
    {std::lock_guard<std::mutex> lock(grant_mutex);if(pending_grant)std::fill(pending_grant->credential.begin(),pending_grant->credential.end(),'\0');pending_grant.reset();}
    state.socket.Close();state.grant_connected=false;
    bool stopped=state.tasks.Stop();
    if(stopped){ReleaseAudio(AudioOwner::Mira);state.status=nullptr;}
    return stopped && usb_closed;
}
void TickView() {
    auto& state=runtime();if(!state.status)return;
    auto& provisioner=state.provisioner;auto& tasks=state.tasks;auto& socket=state.socket;
    auto& wifi_started=state.wifi_started;auto& grant_connected=state.grant_connected;
    auto& connect_gate=state.connect_gate;auto& message=state.message;auto status=state.status;
    uint64_t now=esp_timer_get_time()/1000;
    if(reject_requested.exchange(false))mute_requested=true;
    if(state.numeric_until){
        state.numeric_pairing.Tick(now);
        if(now>=state.numeric_until||(state.numeric_connected&&!state.numeric_pairing.Active(now)))mute_requested=true;
    }
    {std::lock_guard<std::mutex> lock(grant_mutex);if(pending_grant&&pending_grant->expires_at_ms&&now>=pending_grant->expires_at_ms){std::fill(pending_grant->credential.begin(),pending_grant->credential.end(),'\0');pending_grant.reset();mute_requested=true;}}
        if(mute_requested.exchange(false)) {
            provisioner.Close();state.numeric_pairing.Cancel();state.numeric_request={};state.numeric_until=0;state.numeric_connected=false;confirm_requested=false;connect_gate.Cancel();pair_requested=false;connect_requested=false;listen_requested=false;
            {std::lock_guard<std::mutex> lock(grant_mutex);if(pending_grant)std::fill(pending_grant->credential.begin(),pending_grant->credential.end(),'\0');pending_grant.reset();}
            socket.Close();if(tasks.Stop())ReleaseAudio(AudioOwner::Mira);grant_connected=false;message="Mira companion\nMuted - connection closed";}
        if(pair_requested.exchange(false)) {
            connect_gate.Cancel();
            bool pending=false; {std::lock_guard<std::mutex> lock(grant_mutex);pending=pending_grant.has_value();}
            if(grant_connected || pending) message="Mira companion\nMuted - tap Mute before pairing";
            else {
                state.numeric_pairing.Cancel();state.numeric_request={};state.numeric_until=0;state.numeric_connected=false;
                bool opened=provisioner.Open();
                if(opened&&provisioner.Numeric())state.numeric_until=now+120000;
                message=opened?"Mira companion\nPairing ready - keep Mac open":"Mira companion\nMuted - trusted provisioning unavailable";
            }
        }
        if(provisioner.Active()&&provisioner.Numeric()) {
            NumericUsbRequest request;auto result=provisioner.PollNumeric(request);
            if(result==NumericUsbRead::Accepted){
                state.numeric_request=std::move(request);
                if(!wifi_started)wifi_started=StartSavedWifi();
                else if(!connected)esp_wifi_connect();
                if(!wifi_started)mute_requested=true;
                message="Mira companion\nPairing - connecting saved Wi-Fi";
            }else if(result==NumericUsbRead::Rejected){mute_requested=true;message="Mira companion\nNumeric request expired or rejected";}
        }
        if(state.numeric_until&&!state.numeric_request.request.empty()&&!state.numeric_connected&&wifi_started&&connected){
            auto network=Board::GetInstance().GetNetwork();
            uint64_t start=state.numeric_until-120000;
            state.numeric_connected=network&&state.numeric_pairing.Start(*network,state.numeric_request.request,state.numeric_request.boot_nonce,start);
            if(!state.numeric_connected)mute_requested=true;
            else message="Mira companion\nWaiting for matching code";
        }
        if(confirm_requested.exchange(false)){if(!state.numeric_pairing.Confirm(now))mute_requested=true;else message="Mira companion\nRobot confirmed - confirm on Mac";}
        NumericGrant numeric_grant;
        if(state.numeric_pairing.TakeGrant(numeric_grant)){
            AuthorizedGrant grant;grant.uri="wss://192.168.1.75:18443/companion/audio";grant.device="stackchan-80456b561404";grant.request=numeric_grant.request;grant.credential=std::move(numeric_grant.credential);grant.host_audio_qualified=true;grant.expires_at_ms=numeric_grant.expires_at_ms;grant.numeric_pairing=true;
            bool staged=false;
            if(state.numeric_pairing.Active(now))staged=StageAuthorizedGrant(std::move(grant));
            else std::fill(grant.credential.begin(),grant.credential.end(),'\0');
            if(!staged||!state.numeric_pairing.Staged())mute_requested=true;
            else message="Mira companion\nPaired - tap Connect when ready";
        }
        if(provisioner.Active()&&!provisioner.Numeric()) {
            ProvisioningFields fields; auto result=provisioner.Poll(fields);
            if(result==ProvisioningRead::Accepted) {
                AuthorizedGrant grant;grant.uri=fields.uri;grant.device=fields.device;grant.credential=std::move(fields.credential);grant.request=fields.request;
                grant.host_audio_qualified=STACKCHAN_COMPANION_TRUSTED_USB_PROVISIONING;
                bool staged=StageAuthorizedGrant(std::move(grant));
                bool receipt_sent=provisioner.CompleteAccepted(staged,fields.request);
                if(!receipt_sent&&staged){
                    std::lock_guard<std::mutex> lock(grant_mutex);
                    if(pending_grant)std::fill(pending_grant->credential.begin(),pending_grant->credential.end(),'\0');
                    pending_grant.reset();staged=false;
                }
                message=staged?"Mira companion\nMuted - grant staged; tap Connect":"Mira companion\nMuted - grant or acknowledgement rejected";
            } else if(result==ProvisioningRead::Rejected) message="Mira companion\nMuted - pairing expired or rejected";
            fields.Clear();
        }
        if(connect_requested.exchange(false)) {
            bool usb_closed=provisioner.Close();
            bool have_grant=false;{std::lock_guard<std::mutex> lock(grant_mutex);have_grant=pending_grant.has_value();}
            connect_gate.Request(usb_closed&&have_grant);
            if(!usb_closed)message="Mira companion\nMuted - USB cleanup pending; tap Mute";
            else if(!have_grant)message="Mira companion\nMuted - trusted host grant required";
            else if(!wifi_started){wifi_started=StartSavedWifi();message=wifi_started?"Mira companion\nConnecting saved Wi-Fi":"Mira companion\nMuted - Wi-Fi unavailable";}
            else if(!connected){esp_wifi_connect();message="Mira companion\nConnecting saved Wi-Fi";}
        }
        if(connect_gate.Ready(wifi_started && connected) && !grant_connected) {
            std::optional<AuthorizedGrant> grant;{std::lock_guard<std::mutex> lock(grant_mutex);grant=std::move(pending_grant);pending_grant.reset();}
            if(grant) {
                connect_gate.Cancel();
                if(grant->expires_at_ms&&static_cast<uint64_t>(esp_timer_get_time()/1000)>=grant->expires_at_ms){std::fill(grant->credential.begin(),grant->credential.end(),'\0');mute_requested=true;return;}
                auto network=Board::GetInstance().GetNetwork();
                grant_connected=network && socket.Connect(*network,grant->uri,grant->device,grant->credential,grant->request);
                uint64_t after_connect=esp_timer_get_time()/1000;
                if(grant_connected&&((grant->expires_at_ms&&after_connect>=grant->expires_at_ms)||(grant->numeric_pairing&&!state.numeric_pairing.Active(after_connect)))){socket.Close();grant_connected=false;}
                if(grant_connected)grant_connected=AcquireAudio(AudioOwner::Mira) && tasks.Start(Board::GetInstance().GetAudioCodec(),[&socket](const uint8_t* p,size_t n){return socket.SendAudio(p,n);},[](uint32_t,bool){mute_requested=true;});
                std::fill(grant->credential.begin(),grant->credential.end(),'\0');
                if(!grant_connected){socket.Close();if(tasks.Stop())ReleaseAudio(AudioOwner::Mira);message="Mira companion\nMuted - host connection failed";}
            }
        }
        if(listen_requested.exchange(false)) {
            if(!grant_connected || !socket.PhysicalConsent())message="Mira companion\nMuted - host session not ready";
        }
        if(grant_connected) {
            socket.Tick();using S=CompanionSocket::State;
            switch(socket.GetState()) {
                case S::AwaitingBegin:message="Mira companion\nMuted - waiting for host session";break;
                case S::ConsentRequired:message="Mira companion\nMuted - tap Allow to listen";break;
                case S::Arming:message="Mira companion\nMuted - waiting for host approval";break;
                case S::Listening:message="Mira companion\nListening";break;
                case S::Speaking:message="Mira companion\nSpeaking";break;
                case S::Muted:case S::Fault:case S::Disconnected:mute_requested=true;break;
            }
        }
        {LvglLockGuard lock;
            bool compare=!state.numeric_pairing.Code().empty();
            for(auto item:{state.pair_button,state.listen_button,state.connect_button,state.mute_button}){if(compare)lv_obj_add_flag(item,LV_OBJ_FLAG_HIDDEN);else lv_obj_remove_flag(item,LV_OBJ_FLAG_HIDDEN);}
            for(auto item:{state.confirm_button,state.reject_button}){if(compare)lv_obj_remove_flag(item,LV_OBJ_FLAG_HIDDEN);else lv_obj_add_flag(item,LV_OBJ_FLAG_HIDDEN);}
            if(state.numeric_pairing.Confirmed())lv_obj_add_state(state.confirm_button,LV_STATE_DISABLED);else lv_obj_remove_state(state.confirm_button,LV_STATE_DISABLED);
            lv_obj_set_width(status,compare?290:240);lv_obj_set_height(status,compare?90:54);
            std::string display=compare?"Compare code: "+state.numeric_pairing.Code()+(state.numeric_pairing.Confirmed()?"\nRobot confirmed - confirm on Mac":"\nConfirm on Mac and robot"):message;lv_label_set_text(status,display.c_str());
        }
 }
[[noreturn]] void RunBoot() {
    if(!GetHAL().initCompanion())while(true)GetHAL().delay(1000);
    OpenView(lv_screen_active());
    while(true){TickView();GetHAL().delay(20);}
}
}
