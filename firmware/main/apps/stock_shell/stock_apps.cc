// Stock Mooncake shell, original native Tools and game implementations.
#include "stock_apps.h"
#include "stock_tools_hw.h"
#include <apps/apps.h>
#include <apps/common/common.h>
#include <assets/assets.h>
#include <companion/boot.h>
#include <hal/hal.h>
#include <stackchan/stackchan.h>
#include <stackchan/avatar/skins/default/default.h>
#include <esp_random.h>
#include <esp_timer.h>
#include <lvgl.h>
#include <atomic>
#include <memory>
#include <cstdio>
namespace stock_shell {
class Page : public mooncake::AppAbility {
    struct Action {
        Page* owner;
        int id;
    };
    std::vector<std::unique_ptr<Action>> callbacks_;
    bool exit_pending_ = false;

protected:
    lv_obj_t* root_ = nullptr;
    std::atomic<int> action_{0};
    void Panel(const char* title)
    {
        exit_pending_ = false;
        action_       = 0;
        LvglLockGuard lock;
        root_ = lv_obj_create(lv_screen_active());
        lv_obj_set_size(root_, 320, 240);
        lv_obj_set_pos(root_, 0, 0);
        lv_obj_set_style_pad_all(root_, 0, 0);
        lv_obj_remove_flag(root_, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_bg_color(root_, lv_color_hex(0x10232d), 0);
        lv_obj_set_style_text_color(root_, lv_color_hex(0xffffff), 0);
        auto text = lv_label_create(root_);
        lv_label_set_text(text, title);
        lv_obj_set_pos(text, 10, 6);
        Button("Back", 260, 2, 56, 28, 999);
    }
    lv_obj_t* Text(const char* text, int x, int y, int width)
    {
        auto label = lv_label_create(root_);
        lv_label_set_text(label, text);
        lv_obj_set_pos(label, x, y);
        lv_obj_set_width(label, width);
        return label;
    }
    void Button(const char* name, int x, int y, int w, int h, int id)
    {
        auto button = lv_button_create(root_);
        lv_obj_set_pos(button, x, y);
        lv_obj_set_size(button, w, h);
        auto label = lv_label_create(button);
        lv_label_set_text(label, name);
        lv_obj_center(label);
        auto ctx   = std::make_unique<Action>();
        ctx->owner = this;
        ctx->id    = id;
        lv_obj_add_event_cb(
            button,
            [](lv_event_t* event) {
                auto a            = static_cast<Action*>(lv_event_get_user_data(event));
                a->owner->action_ = a->id;
            },
            LV_EVENT_CLICKED, ctx.get());
        callbacks_.push_back(std::move(ctx));
    }
    virtual bool StopResources()
    {
        return true;
    }
    bool Back(int action)
    {
        if (action == 999) exit_pending_ = true;
        if (!exit_pending_) return false;
        if (StopResources()) {
            exit_pending_ = false;
            close();
        }
        return true;
    }

public:
    void onClose() override
    {
        StopResources();
        LvglLockGuard lock;
        if (root_) lv_obj_delete(root_);
        root_ = nullptr;
        callbacks_.clear();
        action_ = 0;
    }
};
class CompanionApp : public Page {
    bool StopResources() override
    {
        return companion::CloseView();
    }

public:
    CompanionApp()
    {
        setAppInfo().name = "COMPANION";
        static auto icon  = assets::get_image("icon_ai_agent.bin");
        setAppInfo().icon = &icon;
    }
    void onOpen() override
    {
        Panel("");
        companion::OpenView(root_);
    }
    void onRunning() override
    {
        auto action = action_.exchange(0);
        if (Back(action)) return;
        companion::TickView();
    }
};
class GatedApp : public Page {
    const char* reason_;

public:
    GatedApp(const mooncake::AppAbility::AppInfo_t& info, const char* reason) : reason_(reason)
    {
        setAppInfo() = info;
    }
    void onOpen() override
    {
        Panel(getAppInfo().name.c_str());
        LvglLockGuard lock;
        Text(reason_, 12, 48, 294);
    }
    void onRunning() override
    {
        Back(action_.exchange(0));
    }
};
class AvatarApp : public Page {
public:
    AvatarApp()
    {
        AppAvatar source;
        setAppInfo() = source.getAppInfo();
    }
    void onOpen() override
    {
        Panel("Local avatar");
        LvglLockGuard lock;
        auto avatar = std::make_unique<stackchan::avatar::DefaultAvatar>();
        avatar->init(root_);
        GetStackChan().attachAvatar(std::move(avatar));
        Button("Back", 260, 2, 56, 28, 999);
    }
    void onRunning() override
    {
        if (Back(action_.exchange(0))) return;
        LvglLockGuard lock;
        GetStackChan().update();
    }
    void onClose() override
    {
        {
            LvglLockGuard lock;
            GetStackChan().clearModifiers();
            GetStackChan().resetAvatar();
        }
        Page::onClose();
    }
};
class ToolsApp : public Page {
    stock_tools::Infrared ir_;
    stock_tools::NfcUrl nfc_;
    lv_obj_t* status_   = nullptr;
    lv_obj_t* url_      = nullptr;
    lv_obj_t* keyboard_ = nullptr;
    bool StopResources() override
    {
        ir_.Stop();
        return nfc_.Stop();
    }
    void Status(const char* text)
    {
        LvglLockGuard lock;
        lv_label_set_text(status_, text);
    }

public:
    ToolsApp()
    {
        setAppInfo().name = "TOOLS";
        static auto icon  = assets::get_image("icon_controller.bin");
        setAppInfo().icon = &icon;
    }
    void onOpen() override
    {
        Panel("Tools - inactive");
        LvglLockGuard lock;
        Button("Learn IR 10s", 8, 36, 146, 36, 1);
        Button("Send learned once", 164, 36, 146, 36, 2);
        url_ = lv_textarea_create(root_);
        lv_obj_set_pos(url_, 8, 82);
        lv_obj_set_size(url_, 302, 38);
        lv_textarea_set_one_line(url_, true);
        lv_textarea_set_max_length(url_, 96);
        lv_textarea_set_placeholder_text(url_, "https://your-url");
        Button("Edit URL", 8, 128, 94, 34, 3);
        Button("Emit NFC 60s", 108, 128, 142, 34, 4);
        Button("Stop", 256, 128, 56, 34, 5);
        status_ = Text("IR and NFC off. Actions require a tap.\nURL and learned IR stay in RAM.", 10, 176, 300);
    }
    void onRunning() override
    {
        auto action = action_.exchange(0);
        if (Back(action)) return;
        auto now = static_cast<uint64_t>(esp_timer_get_time() / 1000);
        if (action == 1) {
            if (!nfc_.Stop()) {
                Status("NFC stop failed; retry Stop");
                return;
            }
            Status(ir_.Learn(now) ? "IR learning for 10s - camera/motion off" : "IR start failed");
        }
        if (action == 2) {
            if (!nfc_.Stop()) {
                Status("NFC stop failed; retry Stop");
                return;
            }
            Status(ir_.SendOnce() ? "One IR transmission completed" : "No bounded learned frame / send failed");
        }
        if (action == 3 && !keyboard_) {
            LvglLockGuard lock;
            keyboard_ = lv_keyboard_create(root_);
            lv_obj_set_size(keyboard_, 320, 118);
            lv_keyboard_set_textarea(keyboard_, url_);
            lv_obj_add_event_cb(
                keyboard_,
                [](lv_event_t* e) {
                    auto code = lv_event_get_code(e);
                    if (code == LV_EVENT_READY || code == LV_EVENT_CANCEL)
                        static_cast<ToolsApp*>(lv_event_get_user_data(e))->action_ = 6;
                },
                LV_EVENT_ALL, this);
        }
        if (action == 6 && keyboard_) {
            LvglLockGuard lock;
            lv_obj_delete(keyboard_);
            keyboard_ = nullptr;
        }
        if (action == 4) {
            ir_.Stop();
            std::string url;
            {
                LvglLockGuard lock;
                url = lv_textarea_get_text(url_);
            }
            Status(nfc_.Start(url, now) ? "NFC URL emulation active for 60s"
                                        : "URL invalid / NFC start failed; tap Stop");
        }
        if (action == 5) Status(StopResources() ? "IR and NFC stopped" : "NFC stop failed; retry Stop before leaving");
        bool learning = ir_.Active();
        if (ir_.Poll(now)) {
            char message[120];
            uint32_t value = 0;
            if (stock_tools::NecDecode(ir_.learned, value))
                std::snprintf(message, sizeof(message), "Learned NEC %08lx. Tap Send for one replay.",
                              static_cast<unsigned long>(value));
            else
                std::snprintf(message, sizeof(message), "Learned %u raw timings. Replay at38kHz only.",
                              static_cast<unsigned>(ir_.learned.size()));
            Status(message);
        } else if (learning && !ir_.Active())
            Status("IR window closed; no valid bounded frame");
        bool emitting = nfc_.Active();
        nfc_.Poll(now);
        if (emitting && !nfc_.Active())
            Status(nfc_.Stop() ? "NFC window expired; stopped" : "NFC stop failed; retry Stop");
    }
    void onClose() override
    {
        Page::onClose();
        ir_.learned.clear();
        status_   = nullptr;
        url_      = nullptr;
        keyboard_ = nullptr;
    }
};
class GamesApp : public Page {
    stock_tools::Game2048 game_;
    std::array<lv_obj_t*, 16> tiles_{};
    lv_obj_t* score_ = nullptr;
    void Render()
    {
        LvglLockGuard lock;
        for (int i = 0; i < 16; ++i) {
            char text[8];
            if (game_.cells[i])
                std::snprintf(text, sizeof(text), "%u", game_.cells[i]);
            else
                text[0] = 0;
            lv_label_set_text(tiles_[i], text);
        }
        char text[64];
        std::snprintf(text, sizeof(text), "2048 | %lu%s", static_cast<unsigned long>(game_.score),
                      game_.CanMove() ? "" : " | Game over");
        lv_label_set_text(score_, text);
    }

public:
    GamesApp()
    {
        setAppInfo().name = "GAMES";
        static auto icon  = assets::get_image("icon_dance.bin");
        setAppInfo().icon = &icon;
    }
    void onOpen() override
    {
        Panel("Games");
        {
            LvglLockGuard lock;
            score_ = Text("", 8, 30, 300);
            for (int y = 0; y < 4; ++y)
                for (int x = 0; x < 4; ++x) {
                    auto cell = lv_obj_create(root_);
                    lv_obj_set_pos(cell, 8 + x * 47, 54 + y * 40);
                    lv_obj_set_size(cell, 44, 37);
                    lv_obj_remove_flag(cell, LV_OBJ_FLAG_SCROLLABLE);
                    tiles_[y * 4 + x] = lv_label_create(cell);
                    lv_obj_center(tiles_[y * 4 + x]);
                }
            Button("Up", 224, 65, 82, 30, 3);
            Button("Left", 205, 105, 54, 30, 1);
            Button("Right", 261, 105, 54, 30, 2);
            Button("Down", 224, 145, 82, 30, 4);
            Button("New", 224, 188, 82, 30, 5);
            game_.Reset();
        }
        Render();
    }
    void onRunning() override
    {
        auto action = action_.exchange(0);
        if (Back(action)) return;
        if (action == 5) {
            game_.Reset();
            Render();
        } else if (action >= 1 && action <= 4) {
            if (game_.Move(action - 1)) game_.AddTile(esp_random());
            Render();
        }
    }
};
[[noreturn]] void Run()
{
    if (!GetHAL().initCompanion())
        while (true) GetHAL().delay(1000);
    smooth_ui_toolkit::ui_hal::on_delay([](uint32_t ms) { GetHAL().delay(ms); });
    smooth_ui_toolkit::ui_hal::on_get_tick([]() { return GetHAL().millis(); });
    auto& shell = mooncake::GetMooncake();
    shell.installApp(std::make_unique<AppLauncher>());
    shell.installApp(std::make_unique<CompanionApp>());
    AppAiAgent ai;
    shell.installApp(
        std::make_unique<GatedApp>(ai.getAppInfo(),
                                   "Vendor AI is retained in source.\nThis build opens Companion instead of activating "
                                   "vendor cloud or wake-word audio.\nReturn and choose COMPANION."));
    shell.installApp(std::make_unique<AvatarApp>());
    AppEspnowControl remote;
    shell.installApp(std::make_unique<GatedApp>(remote.getAppInfo(),
                                                "Stock ESP-NOW control is retained.\nRadio pairing and remote motor "
                                                "control remain inactive pending separate feature qualification."));
    AppAppCenter center;
    shell.installApp(std::make_unique<GatedApp>(center.getAppInfo(),
                                                "Stock App Center is retained.\nOnline downloads and executable app "
                                                "activation require a separately qualified route."));
    AppEzdata data;
    shell.installApp(std::make_unique<GatedApp>(
        data.getAppInfo(), "Stock EzData is retained.\nAccount binding and vendor data transfer remain inactive."));
    AppDance dance;
    shell.installApp(std::make_unique<GatedApp>(dance.getAppInfo(),
                                                "Stock Dance is retained.\nMotors and BLE control remain inactive. "
                                                "Motion requires separate per-feature qualification and consent."));
    shell.installApp(std::make_unique<AppSetup>());
    shell.installApp(std::make_unique<ToolsApp>());
    shell.installApp(std::make_unique<GamesApp>());
    while (true) {
        GetHAL().feedTheDog();
        shell.update();
        GetHAL().delay(20);
    }
}
}  // namespace stock_shell
