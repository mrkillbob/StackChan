#include "boot.h"
#include "audio_owner.hpp"
#include "usb_owner.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "connect_gate.hpp"
#include <driver/usb_serial_jtag.h>
#include <esp_mac.h>
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
};
class UsbProvisioner {
    UsbSdkDriver driver_;
    UsbProvisioningOwner<UsbSdkDriver> owner_{driver_};
public:
    bool Close() {
        bool closed=owner_.Close();
        if(!closed) ESP_LOGW("CompanionUSB","USB owner cleanup failed (%d)",static_cast<int>(owner_.LastError()));
        return closed;
    }
    bool Active() const { return owner_.Active(); }
    bool Open() {
        if(!STACKCHAN_COMPANION_TRUSTED_USB_PROVISIONING)return false;
        uint8_t mac[6];if(esp_read_mac(mac,ESP_MAC_WIFI_STA)!=ESP_OK)return false;
        char device[32];std::snprintf(device,sizeof(device),"stackchan-%02x%02x%02x%02x%02x%02x",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
        return owner_.Open(esp_timer_get_time()/1000,device,true);
    }
    ProvisioningRead Poll(ProvisioningFields& fields) { return owner_.Poll(esp_timer_get_time()/1000,fields); }
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
void button(lv_obj_t* screen,const char* text,int x,std::atomic<bool>& action) {
    auto b=lv_button_create(screen);lv_obj_set_size(b,140,48);lv_obj_align(b,LV_ALIGN_BOTTOM_MID,x,-12);
    auto label=lv_label_create(b);lv_label_set_text(label,text);lv_obj_center(label);
    lv_obj_add_event_cb(b,[](lv_event_t* e){static_cast<std::atomic<bool>*>(lv_event_get_user_data(e))->store(true);},LV_EVENT_CLICKED,&action);
}
}
bool StageAuthorizedGrant(AuthorizedGrant grant) {
    if(!grant.host_audio_qualified || grant.credential.empty() || grant.credential.size()>256 || grant.uri.rfind("wss://",0)!=0 || grant.device.empty() || grant.request.empty()){std::fill(grant.credential.begin(),grant.credential.end(),'\0');return false;}
    std::lock_guard<std::mutex> lock(grant_mutex);if(pending_grant){std::fill(grant.credential.begin(),grant.credential.end(),'\0');return false;}
    pending_grant=std::move(grant);return true;
}
namespace {
// Persistent boot-lifetime owner: never destroy callbacks while workers unwind.
struct Runtime {
    UsbProvisioner provisioner;
    CodecTasks tasks;
    CompanionSocket socket{tasks};
    bool wifi_started=false,grant_connected=false;
    ConnectGate connect_gate;
    lv_obj_t* status=nullptr;
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
    pair_requested=false;connect_requested=false;listen_requested=false;mute_requested=false;
    {LvglLockGuard lock;auto screen=parent;
     lv_obj_set_style_bg_color(screen,lv_color_hex(0x10232d),0);
     status=lv_label_create(screen);lv_obj_set_width(status,240);lv_obj_set_height(status,54);lv_label_set_long_mode(status,LV_LABEL_LONG_DOT);lv_obj_align(status,LV_ALIGN_TOP_LEFT,8,34);
     lv_obj_set_style_text_color(status,lv_color_hex(0xffffff),0);
     lv_label_set_text(status,"Mira companion\nMuted - pair a trusted host\nCamera and motion inactive");
     auto pair=lv_button_create(screen);lv_obj_set_size(pair,290,32);lv_obj_align(pair,LV_ALIGN_CENTER,0,-12);
     auto pair_label=lv_label_create(pair);lv_label_set_text(pair_label,"Pair trusted host");lv_obj_center(pair_label);
     lv_obj_add_event_cb(pair,[](lv_event_t*){pair_requested=true;},LV_EVENT_CLICKED,nullptr);
     button(screen,"Connect",-76,connect_requested);button(screen,"Mute",76,mute_requested);
     auto listen=lv_button_create(screen);lv_obj_set_size(listen,290,48);lv_obj_align(listen,LV_ALIGN_CENTER,0,32);
     auto label=lv_label_create(listen);lv_label_set_text(label,"Allow this conversation");lv_obj_center(label);
     lv_obj_add_event_cb(listen,[](lv_event_t*){listen_requested=true;},LV_EVENT_CLICKED,nullptr);
    }
    state.message="Mira companion\nMuted - pair a trusted host\nCamera and motion inactive";
}
bool CloseView() {
    auto& state=runtime();bool usb_closed=state.provisioner.Close();state.connect_gate.Cancel();
    pair_requested=false;connect_requested=false;listen_requested=false;mute_requested=false;
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
        if(mute_requested.exchange(false)) {
            provisioner.Close();connect_gate.Cancel();pair_requested=false;connect_requested=false;listen_requested=false;
            {std::lock_guard<std::mutex> lock(grant_mutex);if(pending_grant)std::fill(pending_grant->credential.begin(),pending_grant->credential.end(),'\0');pending_grant.reset();}
            socket.Close();if(tasks.Stop())ReleaseAudio(AudioOwner::Mira);grant_connected=false;message="Mira companion\nMuted - connection closed";}
        if(pair_requested.exchange(false)) {
            connect_gate.Cancel();
            bool pending=false; {std::lock_guard<std::mutex> lock(grant_mutex);pending=pending_grant.has_value();}
            if(grant_connected || pending) message="Mira companion\nMuted - tap Mute before pairing";
            else message=provisioner.Open()?"Mira companion\nMuted - USB pairing open for 60s":"Mira companion\nMuted - trusted provisioning unavailable";
        }
        if(provisioner.Active()) {
            ProvisioningFields fields; auto result=provisioner.Poll(fields);
            if(result==ProvisioningRead::Accepted) {
                AuthorizedGrant grant;grant.uri=fields.uri;grant.device=fields.device;grant.credential=std::move(fields.credential);grant.request=fields.request;
                grant.host_audio_qualified=STACKCHAN_COMPANION_TRUSTED_USB_PROVISIONING;
                message=StageAuthorizedGrant(std::move(grant))?"Mira companion\nMuted - grant staged; tap Connect":"Mira companion\nMuted - grant rejected";
            } else if(result==ProvisioningRead::Rejected) message="Mira companion\nMuted - pairing expired or rejected";
            fields.Clear();
        }
        if(connect_requested.exchange(false)) {
            provisioner.Close();
            bool have_grant=false;{std::lock_guard<std::mutex> lock(grant_mutex);have_grant=pending_grant.has_value();}
            connect_gate.Request(have_grant);
            if(!have_grant)message="Mira companion\nMuted - trusted host grant required";
            else if(!wifi_started){wifi_started=StartSavedWifi();message=wifi_started?"Mira companion\nConnecting saved Wi-Fi":"Mira companion\nMuted - Wi-Fi unavailable";}
            else if(!connected){esp_wifi_connect();message="Mira companion\nConnecting saved Wi-Fi";}
        }
        if(connect_gate.Ready(wifi_started && connected) && !grant_connected) {
            std::optional<AuthorizedGrant> grant;{std::lock_guard<std::mutex> lock(grant_mutex);grant=std::move(pending_grant);pending_grant.reset();}
            if(grant) {
                connect_gate.Cancel();
                auto network=Board::GetInstance().GetNetwork();
                grant_connected=network && socket.Connect(*network,grant->uri,grant->device,grant->credential,grant->request);
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
        {LvglLockGuard lock;lv_label_set_text(status,message);}
 }
[[noreturn]] void RunBoot() {
    if(!GetHAL().initCompanion())while(true)GetHAL().delay(1000);
    OpenView(lv_screen_active());
    while(true){TickView();GetHAL().delay(20);}
}
}
