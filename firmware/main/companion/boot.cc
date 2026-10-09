#include "boot.h"
#include "provisioning_receiver.hpp"
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
std::atomic<bool> pair_requested{false}, connect_requested{false}, listen_requested{false}, mute_requested{false}, connected{false};
// Called only by the pinned boot task. Never take over another installed driver.
class UsbProvisioner {
    ProvisioningReceiver receiver_;
    bool installed_=false;
public:
    void Close() { receiver_.Close(); if(installed_) { usb_serial_jtag_driver_uninstall(); installed_=false; } }
    bool Active() const { return receiver_.Active(); }
    bool Open() {
        Close();
        if(!STACKCHAN_COMPANION_TRUSTED_USB_PROVISIONING || usb_serial_jtag_is_driver_installed()) return false;
        uint8_t mac[6]; if(esp_read_mac(mac,ESP_MAC_WIFI_STA)!=ESP_OK) return false;
        char device[32]; std::snprintf(device,sizeof(device),"stackchan-%02x%02x%02x%02x%02x%02x",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
        usb_serial_jtag_driver_config_t config={}; config.rx_buffer_size=1024; config.tx_buffer_size=64;
        if(usb_serial_jtag_driver_install(&config)!=ESP_OK) return false;
        installed_=true;
        uint8_t stale[128]; // Discard a bounded amount queued before physical consent.
        for(int i=0;i<8;++i) { int n=usb_serial_jtag_read_bytes(stale,sizeof(stale),0); std::fill(std::begin(stale),std::end(stale),0); if(n<=0) break; }
        if(!receiver_.Open(esp_timer_get_time()/1000,device,true)) { Close(); return false; }
        return true;
    }
    ProvisioningRead Poll(ProvisioningFields& fields) {
        uint8_t bytes[128]; int n=usb_serial_jtag_read_bytes(bytes,sizeof(bytes),0);
        auto result=receiver_.Feed(bytes,n>0?static_cast<size_t>(n):0,esp_timer_get_time()/1000,fields);
        std::fill(std::begin(bytes),std::end(bytes),0);
        if(result!=ProvisioningRead::Waiting) Close();
        return result;
    }
};
void wifi_event(void*, esp_event_base_t base, int32_t id, void*) {
    if(base==IP_EVENT && id==IP_EVENT_STA_GOT_IP) connected=true;
    if(base==WIFI_EVENT && id==WIFI_EVENT_STA_DISCONNECTED) {connected=false;mute_requested=true;}
}
bool StartSavedWifi() {
    // Read only the existing preferred SSID. Never provisioning/AP or NVS erase.
    nvs_handle_t handle=0; wifi_config_t wifi={};
    if(nvs_open("wifi",NVS_READONLY,&handle)!=ESP_OK)return false;
    size_t ssid_size=sizeof(wifi.sta.ssid), password_size=sizeof(wifi.sta.password);
    auto a=nvs_get_str(handle,"ssid",reinterpret_cast<char*>(wifi.sta.ssid),&ssid_size);
    auto b=nvs_get_str(handle,"password",reinterpret_cast<char*>(wifi.sta.password),&password_size);
    nvs_close(handle);
    if(a!=ESP_OK || b!=ESP_OK || !wifi.sta.ssid[0])return false;
    if(esp_netif_init()!=ESP_OK)return false;
    auto loop=esp_event_loop_create_default();if(loop!=ESP_OK && loop!=ESP_ERR_INVALID_STATE)return false;
    if(!esp_netif_create_default_wifi_sta())return false;
    wifi_init_config_t config=WIFI_INIT_CONFIG_DEFAULT();config.nvs_enable=false;
    if(esp_wifi_init(&config)!=ESP_OK)return false;
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
    if(!grant.host_audio_qualified || grant.credential.empty() || grant.credential.size()>256 || grant.uri.rfind("wss://",0)!=0 || grant.device.empty() || grant.request.empty())return false;
    std::lock_guard<std::mutex> lock(grant_mutex);if(pending_grant)return false;
    pending_grant=std::move(grant);return true;
}
[[noreturn]] void RunBoot() {
    if(!GetHAL().initCompanion()) {
        ESP_LOGE("Companion","NVS unavailable; preserved settings; halted muted");
        while(true)GetHAL().delay(1000);
    }
    lv_obj_t* status=nullptr;
    {LvglLockGuard lock;auto screen=lv_screen_active();lv_obj_clean(screen);
     lv_obj_set_style_bg_color(screen,lv_color_hex(0x10232d),0);
     status=lv_label_create(screen);lv_obj_set_width(status,290);lv_obj_align(status,LV_ALIGN_TOP_MID,0,16);
     lv_obj_set_style_text_color(status,lv_color_hex(0xffffff),0);
     lv_label_set_text(status,"Mira companion\nMuted - pair a trusted host\nCamera and motion inactive");
     auto pair=lv_button_create(screen);lv_obj_set_size(pair,290,32);lv_obj_align(pair,LV_ALIGN_CENTER,0,-28);
     auto pair_label=lv_label_create(pair);lv_label_set_text(pair_label,"Pair trusted host");lv_obj_center(pair_label);
     lv_obj_add_event_cb(pair,[](lv_event_t*){pair_requested=true;},LV_EVENT_CLICKED,nullptr);
     button(screen,"Connect",-76,connect_requested);button(screen,"Mute",76,mute_requested);
     auto listen=lv_button_create(screen);lv_obj_set_size(listen,290,48);lv_obj_align(listen,LV_ALIGN_CENTER,0,20);
     auto label=lv_label_create(listen);lv_label_set_text(label,"Allow this conversation");lv_obj_center(label);
     lv_obj_add_event_cb(listen,[](lv_event_t*){listen_requested=true;},LV_EVENT_CLICKED,nullptr);
    }
    // Lifetime is the boot loop. No destructor races with codec/network workers.
    UsbProvisioner provisioner; CodecTasks tasks;CompanionSocket socket(tasks);bool wifi_started=false,grant_connected=false;ConnectGate connect_gate;
    const char* message="Mira companion\nMuted - pair a trusted host\nCamera and motion inactive";
    while(true) {
        if(mute_requested.exchange(false)) {
            provisioner.Close();connect_gate.Cancel();pair_requested=false;connect_requested=false;listen_requested=false;
            {std::lock_guard<std::mutex> lock(grant_mutex);if(pending_grant)std::fill(pending_grant->credential.begin(),pending_grant->credential.end(),'\0');pending_grant.reset();}
            socket.Close();tasks.Stop();grant_connected=false;message="Mira companion\nMuted - connection closed";}
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
                if(grant_connected)grant_connected=tasks.Start(Board::GetInstance().GetAudioCodec(),[&socket](const uint8_t* p,size_t n){return socket.SendAudio(p,n);},[](uint32_t,bool){mute_requested=true;});
                std::fill(grant->credential.begin(),grant->credential.end(),'\0');
                if(!grant_connected){socket.Close();tasks.Stop();message="Mira companion\nMuted - host connection failed";}
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
        GetHAL().delay(20);
    }
}
}
