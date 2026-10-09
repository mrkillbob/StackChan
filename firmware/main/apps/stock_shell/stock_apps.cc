// Stock Mooncake shell, original native Tools and game implementations.
#include "stock_apps.h"
#include "menu_policy.hpp"
#include "navigation.h"
#include "shell_teardown.hpp"
#include "stock_tools_hw.h"
#include "tools_ui.hpp"
#include <apps/stock_agent/stock_agent.h>
#include <apps/network_setup/network_setup.h>
#include <apps/stock_services/stock_services.h>
#include <companion/settings_policy.hpp>
#include <map>
#include <optional>
#include <apps/apps.h>
#include <apps/common/common.h>
#include <assets/assets.h>
#include <companion/boot.h>
#include <hal/hal.h>
#include <stackchan/stackchan.h>
#include <stackchan/avatar/skins/default/default.h>
#include <esp_random.h>
#include <esp_system.h>
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
    lv_obj_t* Button(const char* name, int x, int y, int w, int h, int id)
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
        return button;
    }
    void ClearPanel()
    {
        LvglLockGuard lock;
        lv_obj_clean(root_);
        callbacks_.clear();
        action_ = 0;
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
class ToolsApp : public Page {
    using Screen = stock_tools::ui::Screen;
    stock_tools::Infrared ir_;
    stock_tools::NfcUrl nfc_;
    Screen screen_ = Screen::Home;
    std::optional<Screen> pending_screen_;
    lv_obj_t *title_ = nullptr, *summary_ = nullptr, *detail_ = nullptr, *status_ = nullptr, *primary_ = nullptr,
             *url_input_ = nullptr;
    std::string url_, draft_, notice_;
    lv_obj_t* graphic_ = nullptr;
    int cursor_        = 0;
    size_t key_page_   = 0;
    uint64_t deadline_ = 0, next_stop_retry_ = 0;
    bool captured_ = false, stop_failed_ = false;
    bool StopResources() override
    {
        ir_.Stop();
        stop_failed_ = !nfc_.Stop();
        return !stop_failed_;
    }
    lv_obj_t* Label(const char* text, int y, int height, const lv_font_t* font = &lv_font_montserrat_20)
    {
        auto label = Text(text, 8, y, 304);
        lv_obj_set_height(label, height);
        lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
        lv_obj_set_style_text_font(label, font, 0);
        return label;
    }
    lv_obj_t* TouchButton(const char* name, stock_tools::ui::Rect rect, int action)
    {
        auto button = Button(name, rect.x, rect.y, rect.w, rect.h, action);
        lv_obj_set_style_radius(button, 10, 0);
        lv_obj_set_style_bg_color(button, lv_color_hex(0x22596d), 0);
        lv_obj_set_style_border_width(button, 0, 0);
        lv_obj_set_style_text_font(button, &lv_font_montserrat_20, 0);
        lv_obj_set_style_opa(button, LV_OPA_50, LV_STATE_DISABLED);
        return button;
    }
    lv_obj_t* Graphic(lv_obj_t* parent, int x, int y, bool tag, bool small = false)
    {
        auto graphic = lv_obj_create(parent);
        lv_obj_set_pos(graphic, x, y);
        lv_obj_set_size(graphic, tag ? (small ? 32 : 52) : (small ? 28 : 36), small ? 40 : 56);
        lv_obj_set_style_pad_all(graphic, 0, 0);
        lv_obj_set_style_radius(graphic, tag ? 10 : 6, 0);
        lv_obj_set_style_bg_opa(graphic, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(graphic, 2, 0);
        lv_obj_set_style_border_color(graphic, lv_color_hex(0x53d4d4), 0);
        lv_obj_remove_flag(graphic, static_cast<lv_obj_flag_t>(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));
        auto piece = [&](int px, int py, int w, int h) {
            auto child = lv_obj_create(graphic);
            lv_obj_set_pos(child, px, py);
            lv_obj_set_size(child, w, h);
            lv_obj_set_style_pad_all(child, 0, 0);
            lv_obj_set_style_border_width(child, 0, 0);
            lv_obj_set_style_radius(child, 2, 0);
            lv_obj_set_style_bg_color(child, lv_color_hex(0x53d4d4), 0);
            lv_obj_remove_flag(child, static_cast<lv_obj_flag_t>(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE));
        };
        if (tag) {
            piece(5, 5, 6, 6);
            piece(12, small ? 16 : 20, small ? 12 : 28, 3);
            piece(12, small ? 23 : 28, small ? 12 : 22, 3);
        } else {
            piece(5, 5, small ? 14 : 22, 8);
            for (int i = 0; i < 6; ++i)
                piece(5 + (i % 2) * (small ? 10 : 14), 19 + (i / 2) * (small ? 6 : 9), small ? 6 : 8, 4);
        }
        return graphic;
    }
    void ButtonText(lv_obj_t* button, const char* text)
    {
        lv_label_set_text(lv_obj_get_child(button, 0), text);
    }
    bool ValidUrl() const
    {
        std::array<uint8_t, 180> memory{};
        std::array<uint8_t, 7> uid{};
        return stock_tools::UrlTag(url_, uid, memory);
    }
    std::string SignalSummary() const
    {
        if (ir_.learned.empty()) return "No signal captured";
        uint32_t value = 0;
        char text[80];
        if (stock_tools::NecDecode(ir_.learned, value))
            std::snprintf(text, sizeof(text), "NEC %08lX\n%u timings", static_cast<unsigned long>(value),
                          static_cast<unsigned>(ir_.learned.size()));
        else
            std::snprintf(text, sizeof(text), "Raw IR signal\n%u timings", static_cast<unsigned>(ir_.learned.size()));
        return text;
    }
    void Header(const char* title)
    {
        ClearPanel();
        summary_ = detail_ = status_ = primary_ = url_input_ = graphic_ = nullptr;
        lv_obj_set_style_text_font(root_, &lv_font_montserrat_20, 0);
        TouchButton("Back", stock_tools::ui::BackButton, screen_ == Screen::Home ? 999 : stock_tools::ui::Back);
        title_ = Text(title, 104, 12, 208);
        lv_obj_set_height(title_, 32);
        lv_obj_set_style_text_font(title_, &lv_font_montserrat_24, 0);
        lv_label_set_long_mode(title_, LV_LABEL_LONG_DOT);
    }
    void Render()
    {
        using namespace stock_tools::ui;
        LvglLockGuard lock;
        switch (screen_) {
            case Screen::Home:
                Header("Tools");
                {
                    auto b = TouchButton("Infrared", RowOne, OpenInfrared);
                    lv_obj_align(lv_obj_get_child(b, 0), LV_ALIGN_LEFT_MID, 16, 0);
                    Graphic(b, 260, 10, false, true);
                }
                {
                    auto b = TouchButton("NFC URL", RowTwo, OpenNfc);
                    lv_obj_align(lv_obj_get_child(b, 0), LV_ALIGN_LEFT_MID, 16, 0);
                    Graphic(b, 254, 10, true, true);
                }
                Label("Actions start only when tapped", 212, 24, &lv_font_montserrat_16);
                break;
            case Screen::Infrared:
                Header("Infrared");
                TouchButton("Learn a signal", RowOne, OpenLearn);
                TouchButton("Review & send", RowTwo, OpenSend);
                Label("One signal stays in RAM only", 212, 24, &lv_font_montserrat_16);
                break;
            case Screen::Learn:
                Header("Learn IR");
                summary_ = Label("Ready to learn", 60, 48);
                lv_obj_set_width(summary_, 232);
                graphic_ = Graphic(root_, 270, 60, false);
                detail_  = Label("Aim the remote at the IR sensor.\nPress its button after starting.", 108, 38,
                                 &lv_font_montserrat_16);
                status_  = Label("IR off", 148, 24);
                primary_ = TouchButton("Start 10s scan", Primary, StartLearn);
                break;
            case Screen::Send:
                Header("Send IR");
                summary_ = Label(SignalSummary().c_str(), 60, 52);
                lv_obj_set_width(summary_, 232);
                graphic_ = Graphic(root_, 270, 60, false);
                detail_  = Label("Aim at device. 38kHz, once.", 120, 24, &lv_font_montserrat_16);
                status_  = Label("Nothing sent yet", 148, 24, &lv_font_montserrat_16);
                primary_ = TouchButton("Send once", Primary, SendOnce);
                if (ir_.learned.empty()) lv_obj_add_state(primary_, LV_STATE_DISABLED);
                break;
            case Screen::Nfc:
                Header("NFC URL");
                summary_ = Label(url_.empty() ? "No URL - tap Edit" : url_.c_str(), 56, 40);
                lv_obj_set_width(summary_, 248);
                graphic_ = Graphic(root_, 276, 54, true, true);
                TouchButton("Edit URL", EditButton, OpenEdit);
                primary_ = TouchButton("Emulate 60s", EmulateButton, StartEmulate);
                if (!ValidUrl()) lv_obj_add_state(primary_, LV_STATE_DISABLED);
                break;
            case Screen::Edit: {
                Header("Edit URL");
                url_input_ = lv_textarea_create(root_);
                lv_obj_set_pos(url_input_, UrlInput.x, UrlInput.y);
                lv_obj_set_size(url_input_, UrlInput.w, UrlInput.h);
                lv_textarea_set_one_line(url_input_, true);
                lv_textarea_set_max_length(url_input_, 96);
                lv_textarea_set_placeholder_text(url_input_, "https://");
                lv_textarea_set_text(url_input_, draft_.c_str());
                lv_textarea_set_cursor_pos(url_input_, cursor_);
                for (size_t i = 0; i < KeyPages[key_page_].size(); ++i) {
                    char text[2] = {KeyPages[key_page_][i], 0};
                    TouchButton(text, KeyRect(static_cast<int>(i)), Character + static_cast<unsigned char>(text[0]));
                }
                char page[20];
                std::snprintf(page, sizeof(page), "Keys %u/8", static_cast<unsigned>(key_page_ + 1));
                auto next = TouchButton(page, EditorFooter[0], NextKeys);
                lv_obj_set_style_text_font(next, &lv_font_montserrat_16, 0);
                TouchButton("<--", EditorFooter[1], Erase);
                TouchButton("Save", EditorFooter[2], Save);
                break;
            }
            case Screen::Emulate:
                Header("Emulate");
                summary_ = Label("Hold a reader near\nthe NFC antenna.", 60, 52);
                lv_obj_set_width(summary_, 232);
                graphic_ = Graphic(root_, 254, 60, true);
                status_  = Label("NFC off", 120, 32, &lv_font_montserrat_24);
                detail_  = Label("Stops automatically after 60s", 152, 20, &lv_font_montserrat_16);
                primary_ = TouchButton("Emulate 60s", Primary, StartEmulate);
                break;
        }
        if (!notice_.empty() && status_) lv_label_set_text(status_, notice_.c_str());
    }
    bool Go(Screen target, uint64_t now)
    {
        pending_screen_ = target;
        if (!StopResources()) {
            next_stop_retry_ = now + 500;
            notice_          = "Stop failed - retry Back";
            Refresh(now);
            return false;
        }
        pending_screen_.reset();
        screen_ = target;
        notice_.clear();
        deadline_ = 0;
        Render();
        return true;
    }
    void Refresh(uint64_t now)
    {
        using namespace stock_tools::ui;
        LvglLockGuard lock;
        if (graphic_)
            lv_obj_set_style_border_color(graphic_, lv_color_hex((ir_.Active() || nfc_.Active()) ? 0x69e3a7 : 0x53d4d4),
                                          0);
        if (!status_) return;
        if (stop_failed_) {
            lv_label_set_text(status_, "Stop failed - retry Back");
            if (primary_) {
                ButtonText(primary_, "Retry Stop");
                lv_obj_remove_state(primary_, LV_STATE_DISABLED);
            }
            return;
        }
        if (screen_ == Screen::Learn) {
            if (ir_.Active()) {
                char text[40];
                auto remaining = deadline_ > now ? (deadline_ - now + 999) / 1000 : 0;
                std::snprintf(text, sizeof(text), "Listening - %lus left", static_cast<unsigned long>(remaining));
                lv_label_set_text(summary_, "Press the remote button");
                lv_label_set_text(status_, text);
                ButtonText(primary_, "Stop scan");
            } else if (captured_) {
                lv_label_set_text(summary_, SignalSummary().c_str());
                lv_label_set_text(detail_, "Captured. Review before sending.");
                lv_label_set_text(status_, "IR off");
                ButtonText(primary_, "Review & send");
            } else {
                lv_label_set_text(status_, notice_.empty() ? "IR off" : notice_.c_str());
                ButtonText(primary_, "Start 10s scan");
            }
        } else if (screen_ == Screen::Emulate) {
            if (nfc_.Active()) {
                char text[40];
                auto remaining = deadline_ > now ? (deadline_ - now + 999) / 1000 : 0;
                std::snprintf(text, sizeof(text), "NFC active - %lus", static_cast<unsigned long>(remaining));
                lv_label_set_text(status_, text);
                ButtonText(primary_, "Stop emulation");
            } else {
                lv_label_set_text(status_, notice_.empty() ? "NFC off" : notice_.c_str());
                ButtonText(primary_, "Emulate 60s");
            }
        } else if (!notice_.empty())
            lv_label_set_text(status_, notice_.c_str());
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
        screen_ = Screen::Home;
        url_.clear();
        draft_.clear();
        notice_.clear();
        pending_screen_.reset();
        captured_ = stop_failed_ = false;
        Panel("");
        Render();
    }
    void onRunning() override
    {
        using namespace stock_tools::ui;
        auto action = action_.exchange(0);
        auto now    = static_cast<uint64_t>(esp_timer_get_time() / 1000);
        if (screen_ == Screen::Home && Page::Back(action == stock_tools::ui::Back ? 999 : action)) return;
        if (pending_screen_) {
            if (now >= next_stop_retry_) Go(*pending_screen_, now);
            return;
        }
        if (action && !Allowed(screen_, action)) action = 0;
        if (action == stock_tools::ui::Back) {
            Go(Parent(screen_), now);
            return;
        }
        if (action == OpenInfrared) {
            Go(Screen::Infrared, now);
            return;
        }
        if (action == OpenNfc) {
            Go(Screen::Nfc, now);
            return;
        }
        if (action == OpenLearn) {
            captured_ = false;
            Go(Screen::Learn, now);
            return;
        }
        if (action == OpenSend) {
            Go(Screen::Send, now);
            return;
        }
        if (action == OpenEdit) {
            draft_    = url_;
            key_page_ = 0;
            cursor_   = static_cast<int>(draft_.size());
            Go(Screen::Edit, now);
            return;
        }
        if (screen_ == Screen::Edit) {
            if (action >= Character + 33 && action <= Character + 126) {
                LvglLockGuard lock;
                lv_textarea_add_char(url_input_, static_cast<uint32_t>(action - Character));
            }
            if (action == Erase) {
                LvglLockGuard lock;
                lv_textarea_delete_char(url_input_);
            }
            if (action == NextKeys) {
                {
                    LvglLockGuard lock;
                    draft_  = lv_textarea_get_text(url_input_);
                    cursor_ = lv_textarea_get_cursor_pos(url_input_);
                }
                key_page_ = (key_page_ + 1) % KeyPages.size();
                Render();
            }
            if (action == Save) {
                std::string candidate;
                {
                    LvglLockGuard lock;
                    candidate = lv_textarea_get_text(url_input_);
                }
                std::array<uint8_t, 180> memory{};
                std::array<uint8_t, 7> uid{};
                if (stock_tools::UrlTag(candidate, uid, memory)) {
                    url_ = candidate;
                    Go(Screen::Nfc, now);
                } else {
                    LvglLockGuard lock;
                    lv_label_set_text(title_, "Use http(s) URL");
                    lv_obj_set_style_text_font(title_, &lv_font_montserrat_20, 0);
                }
            }
            return;
        }
        if (action == StartLearn && screen_ == Screen::Learn) {
            if (stop_failed_) {
                StopResources();
                notice_ = "IR off";
            } else if (ir_.Active()) {
                ir_.Stop();
                notice_ = "Scan stopped";
            } else if (captured_) {
                Go(Screen::Send, now);
                return;
            } else {
                captured_ = false;
                notice_   = ir_.Learn(now) ? "" : "IR start failed";
                deadline_ = now + 10000;
            }
        }
        if (action == SendOnce && screen_ == Screen::Send)
            notice_ = ir_.SendOnce() ? "Sent once" : "Send failed / no signal";
        if (action == StartEmulate) {
            if (stop_failed_) {
                StopResources();
                notice_ = "NFC off";
            } else if (nfc_.Active()) {
                notice_ = StopResources() ? "NFC stopped" : "Stop failed";
            } else if (screen_ == Screen::Nfc || screen_ == Screen::Emulate) {
                if (screen_ == Screen::Nfc && !Go(Screen::Emulate, now)) return;
                bool started = nfc_.Start(url_, now);
                notice_      = started ? "" : "NFC start failed";
                if (!started) StopResources();
                deadline_ = now + 60000;
            }
        }
        if (action == Stop) notice_ = StopResources() ? "Stopped" : "Stop failed";
        bool learning = ir_.Active();
        if (ir_.Poll(now)) {
            captured_ = true;
            notice_   = "Captured";
        } else if (learning && !ir_.Active())
            notice_ = "No signal - try again";
        bool emitting = nfc_.Active();
        nfc_.Poll(now);
        if (emitting && !nfc_.Active()) {
            stop_failed_ = !nfc_.Stop();
            notice_      = stop_failed_ ? "Stop failed" : "Window ended - NFC off";
        }
        Refresh(now);
    }
    void onClose() override
    {
        Page::onClose();
        ir_.learned.clear();
        url_.clear();
        draft_.clear();
        pending_screen_.reset();
        title_ = summary_ = detail_ = status_ = primary_ = url_input_ = graphic_ = nullptr;
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
void DestroyShellForTransfer(mooncake::Mooncake& shell)
{
    TearDownShell<LvglLockGuard>(shell,[]{
        view::destroy_home_indicator();
        view::destroy_status_bar();
        view::reset_toast_runtime();
    },[]{mooncake::DestroyMooncake();});
}
[[noreturn]] void Run()
{
    if (!GetHAL().initCompanion())
        while (true) GetHAL().delay(1000);
    smooth_ui_toolkit::ui_hal::on_delay([](uint32_t ms) { GetHAL().delay(ms); });
    smooth_ui_toolkit::ui_hal::on_get_tick([]() { return GetHAL().millis(); });
    auto& shell = mooncake::GetMooncake();
    auto launcher_owned = std::make_unique<AppLauncher>();
    auto* launcher = launcher_owned.get();
    shell.installApp(std::move(launcher_owned));
    // Official 1.5.1 launcher order; setup workers are children of SETUP.
    int stock_agent_id=shell.installApp(stock_agent::MakeApp());
    std::map<stock_services::Feature,int> native_ids;
    using Feature=stock_services::Feature;
    for(auto feature:LauncherNativeFeatures) native_ids[feature]=shell.installApp(stock_services::MakeApp(feature));
    auto setup_owned=std::make_unique<AppSetup>();auto* setup=setup_owned.get();
    int setup_id=shell.installApp(std::move(setup_owned));
    shell.installApp(std::make_unique<CompanionApp>());
    shell.installApp(std::make_unique<ToolsApp>());
    shell.installApp(std::make_unique<GamesApp>());
    bool stopped_preview=false;
    bool setup_return=ConsumeSetupReturn(); // consume even when a more specific preview wins
    if(stock_agent::ConsumePreviewRequest()){stopped_preview=true;launcher->openApp(stock_agent_id);}
    if(network_setup::ConsumePreviewRequest()){
        stopped_preview=true;setup->openInitialWorker([]{return network_setup::MakeWorker();});launcher->openApp(setup_id);
    }
    if(auto feature=stock_services::ConsumePreviewRequest()){
        stopped_preview=true;
        if(IsSetupFeature(*feature)){
            launcher->openApp(setup_id);
        }else if(*feature==Feature::Onboarding){
            setup->openInitialWorker([feature]{return stock_services::MakeWorker(*feature);});launcher->openApp(setup_id);
        }else{
            auto destination=native_ids.find(*feature);
            if(destination!=native_ids.end())launcher->openApp(destination->second);
        }
    }
    if(!stopped_preview&&setup_return){stopped_preview=true;launcher->openApp(setup_id);}
    if(!stopped_preview&&!GetHAL().isAppConfiged()&&esp_reset_reason()!=ESP_RST_SW){
        stopped_preview=true;setup->openInitialWorker([]{return stock_services::MakeWorker(Feature::Onboarding);});launcher->openApp(setup_id);
    }
    if (!stopped_preview && stock_agent::AutoStartRequested()) {
        stock_agent::AuthorizeAutoStart();
        DestroyShellForTransfer(shell);
        stock_agent::RunStockAgent();
    }
    while (true) {
        GetHAL().feedTheDog();
        shell.update();
        if(auto feature=stock_services::ConsumeRequest()) {
            DestroyShellForTransfer(shell);
            companion::SetSettingsScope((*feature==Feature::Servo || *feature==Feature::Onboarding) ? companion::SettingsScope::Calibration :
                *feature==Feature::Account ? companion::SettingsScope::Account :
                (*feature==Feature::Avatar || *feature==Feature::EzData || *feature==Feature::AppCenter || *feature==Feature::FirmwareCheck) ? companion::SettingsScope::RemoteService : companion::SettingsScope::None);
            SetSetupContext(IsSetupFeature(*feature));
            stock_services::RunFeature(*feature);
        }
        if (stock_agent::ConsumeStartRequest()) {
            DestroyShellForTransfer(shell);
            stock_agent::RunStockAgent();
        }
        GetHAL().delay(20);
    }
}
}  // namespace stock_shell
