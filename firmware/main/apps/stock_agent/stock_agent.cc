// SPDX-License-Identifier: MIT
// Intentional feature-entry adapter for the pinned official Stock AI.AGENT lifecycle.
#include "stock_agent.h"
#include "session_controls.hpp"
#include <apps/network_setup/persistence_gate.h>
#include <wifi_manager.h>
#include <apps/app_ai_agent/app_ai_agent.h>
#include <companion/boot.h>
#include <companion/audio_owner.hpp>
#include <companion/settings_policy.hpp>
#include <hal/hal.h>
#include <hal/board/hal_bridge.h>
#include <stackchan/stackchan.h>
#include <application.h>
#include <board.h>
#include <audio_codec.h>
#include <esp_attr.h>
#include <esp_system.h>
#include <lvgl.h>
#include <atomic>

namespace stock_agent {
namespace {
std::atomic<bool> start_requested{false};
std::atomic<bool> start_authorized{false};
RTC_NOINIT_ATTR uint32_t preview_cookie;
constexpr uint32_t preview_magic = 0x53544F50; // STOP: preview, never resume

[[noreturn]] void ReturnHome() {
    GetHAL().setServoPowerEnabled(false);
    GetHAL().setLaserEnabled(false);
    GetHAL().showRgbColor(0,0,0);
    preview_cookie = 0;
    // The official service has no joined teardown. Hardware reset ends all tasks,
    // radio activity and codec use before the stopped selector is presented.
    esp_restart();
    while (true) GetHAL().delay(1000);
}

lv_obj_t* Label(lv_obj_t* parent, const char* value, int x, int y, int width) {
    auto text = lv_label_create(parent);
    lv_label_set_text(text, value);
    lv_obj_set_pos(text, x, y);
    lv_obj_set_width(text, width);
    return text;
}

class StockAgentApp : public mooncake::AppAbility {
    lv_obj_t* panel_ = nullptr;
    lv_obj_t* notice_ = nullptr;
    std::atomic<int> action_{0};
    void Button(const char* text, int x, int y, int width, int action) {
        auto button = lv_button_create(panel_);
        lv_obj_set_pos(button, x, y);
        lv_obj_set_size(button, width, 44);
        auto label = lv_label_create(button);
        lv_label_set_text(label, text);
        lv_obj_center(label);
        // Store action on the button, and object lifetime in the app callback.
        lv_obj_set_user_data(button, reinterpret_cast<void*>(static_cast<intptr_t>(action)));
        lv_obj_add_event_cb(button, [](lv_event_t* e) {
            auto self = static_cast<StockAgentApp*>(lv_event_get_user_data(e));
            auto button = static_cast<lv_obj_t*>(lv_event_get_target(e));
            self->action_ = static_cast<int>(reinterpret_cast<intptr_t>(lv_obj_get_user_data(button)));
        }, LV_EVENT_CLICKED, this);
    }
public:
    StockAgentApp() {
        AppAiAgent stock;
        setAppInfo() = stock.getAppInfo();
    }
    void onOpen() override {
        action_ = 2; // The intentional AI.AGENT tile is the activation action.
        LvglLockGuard lock;
        panel_ = lv_obj_create(lv_screen_active());
        lv_obj_set_size(panel_, 320, 240);
        lv_obj_set_pos(panel_, 0, 0);
        lv_obj_set_style_pad_all(panel_, 0, 0);
        lv_obj_remove_flag(panel_, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_bg_color(panel_, lv_color_hex(0x10232d), 0);
        lv_obj_set_style_text_color(panel_, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_text_font(panel_, &lv_font_montserrat_16, 0);
        Label(panel_, "Opening stock AI.AGENT", 10, 10, 300);
        Label(panel_, "AI.AGENT uses cloud AI, microphone/wake word, camera and motors.\nStop / Home restarts launcher.", 10, 43, 300);
        notice_ = Label(panel_, (companion::NetworkWasStarted() || network_setup::NetworkWasStarted() || WifiManager::GetInstance().IsInitialized())
            ? "Wi-Fi was used. Restart this\nfeature transfer."
            : "Wi-Fi setup: phone browser.\nStock vendor activation.\nWi-Fi changes are saved after connection.",
            10, 128, 300);
        Button("Back", 10, 192, 90, 1);
        Button((companion::NetworkWasStarted() || network_setup::NetworkWasStarted() || WifiManager::GetInstance().IsInitialized()) ? "Retry transfer" : "Retry", 112, 192, 198, 2);
    }
    void onRunning() override {
        auto action = action_.exchange(0);
        if (action == 1) close();
        if (action != 2) return;
        if (companion::CurrentAudioOwner() != companion::AudioOwner::None) {
            LvglLockGuard lock;
            lv_label_set_text(notice_, "Audio is still stopping.\nReopen AI.AGENT when stopped.");
            return;
        }
        if ((companion::NetworkWasStarted() || network_setup::NetworkWasStarted() || WifiManager::GetInstance().IsInitialized())) {
            preview_cookie = preview_magic;
            esp_restart(); // no start request survives reset
            return;
        }
        start_requested = true;
    }
    void onClose() override {
        action_ = 0;
        LvglLockGuard lock;
        if (panel_) lv_obj_delete(panel_);
        panel_ = nullptr;
        notice_ = nullptr;
    }
};

const char* SessionText() {
    // The original HAL background task owns avatar/motion/reminder updates.
    auto state = Application::GetInstance().GetDeviceState();
    const char* text = "Stock AI - starting";
    switch (state) {
        case kDeviceStateWifiConfiguring: text = "Stock AI - Wi-Fi setup"; break;
        case kDeviceStateActivating: text = "Stock AI - activation"; break;
        case kDeviceStateIdle: text = "Stock AI - wake word on"; break;
        case kDeviceStateConnecting: text = "Stock AI - connecting"; break;
        case kDeviceStateListening: text = "Stock AI - listening"; break;
        case kDeviceStateSpeaking: text = "Stock AI - speaking"; break;
        case kDeviceStateAudioTesting: text = "Stock AI - audio test"; break;
        case kDeviceStateFatalError: text = "Stock AI - error"; break;
        default: break;
    }
    return text;
}
void SessionControls() {
    LvglLockGuard lock;
    static CleanSessionControls controls;
    // A compact reveal affordance leaves the native face unobstructed. State and
    // Stop/Home appear only when requested; the original reset path is retained.
    controls.Create(ReturnHome, SessionText);
}
}
std::unique_ptr<mooncake::AppAbility> MakeApp() { return std::make_unique<StockAgentApp>(); }
bool ConsumeStartRequest() {
    bool requested = start_requested.exchange(false);
    if (requested) start_authorized = true;
    return requested;
}
bool ConsumePreviewRequest() {
    bool requested = esp_reset_reason() == ESP_RST_SW && preview_cookie == preview_magic;
    preview_cookie = 0;
    return requested;
}
bool AutoStartRequested() {
    // Legacy boot_ai remains stored, but only intentional feature entry may
    // activate AI.AGENT in this variant. Passive boot/reconnect is not consent.
    return false;
}
void AuthorizeAutoStart() {
    if (AutoStartRequested()) start_authorized = true;
}
static companion::AudioLease runtime_audio_lease;
[[noreturn]] void RunStockAgent() {
    if (!start_authorized.exchange(false) || (companion::NetworkWasStarted() || network_setup::NetworkWasStarted() || WifiManager::GetInstance().IsInitialized()) ||
        !companion::AcquireAudio(companion::AudioOwner::Stock,companion::AudioPurpose::StockRuntime,1,runtime_audio_lease)) ReturnHome();
    companion::SetSettingsScope(companion::SettingsScope::StockRuntime);
    Board::GetInstance().GetUuid();
    hal_bridge::set_xiaozhi_mode(true);
    SessionControls(); // present before audio/network/provisioning activation
    // Pass captured custody before any Application audio initialization.
    if (!Application::GetInstance().GetAudioService().BindAudioLease(runtime_audio_lease)) {
        companion::RevokeAudio(runtime_audio_lease);
        ReturnHome();
    }
    // Root's HAL prepares motion/camera, MCP and the power policy before the
    // official reminder/update task and Application lifecycle begin.
    GetHAL().startXiaozhi();
    ReturnHome();
}
}
