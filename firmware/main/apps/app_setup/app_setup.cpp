/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_setup.h"
#include <hal/hal.h>
#include <mooncake.h>
#include <mooncake_log.h>
#include <assets/assets.h>
#include <stackchan/stackchan.h>
#include <atomic>
#include <apps/common/common.h>

#ifdef STACKCHAN_COMPANION_BOOT
namespace {
class LockedFeatureWorker : public setup_workers::WorkerBase {
    lv_obj_t* root_=nullptr;std::atomic<bool> back_{false};
public:
    explicit LockedFeatureWorker(const std::string& feature) {
        root_=lv_obj_create(lv_screen_active());lv_obj_set_size(root_,320,240);
        auto text=lv_label_create(root_);lv_obj_set_width(text,280);
        std::string message=feature+"\nRetained in stock source.\nThis action needs separate feature qualification; camera, motion, credentials and vendor transfer remain inactive.";
        lv_label_set_text(text,message.c_str());lv_obj_align(text,LV_ALIGN_TOP_MID,0,12);
        auto button=lv_button_create(root_);lv_obj_align(button,LV_ALIGN_BOTTOM_MID,0,-12);auto label=lv_label_create(button);lv_label_set_text(label,"Back");
        lv_obj_add_event_cb(button,[](lv_event_t* e){static_cast<LockedFeatureWorker*>(lv_event_get_user_data(e))->back_=true;},LV_EVENT_CLICKED,this);
    }
    ~LockedFeatureWorker(){lv_obj_delete(root_);}
    void update()override{if(back_.load())_is_done=true;}
};
}
#endif
using namespace mooncake;
using namespace view;
using namespace setup_workers;

AppSetup::AppSetup()
{
    // 配置 App 名
    setAppInfo().name = "SETUP";
    // 配置 App 图标
    static auto icon  = assets::get_image("icon_setup.bin");
    setAppInfo().icon = (void*)&icon;
    // 配置 App 主题颜色
    static uint32_t theme_color = 0xB3B3B3;
    setAppInfo().userData       = (void*)&theme_color;
}

void AppSetup::onCreate()
{
    mclog::tagInfo(getAppInfo().name, "on create");
    // open();
}

void AppSetup::onOpen()
{
    mclog::tagInfo(getAppInfo().name, "on open");

    // Reset state
    _destroy_menu    = false;
    _need_warm_reset = false;
    _magic_count     = 0;

    _menu_sections = {
        {
            "Wi-Fi",
            {{"Change Wi-Fi",
              [&]() {
                  _destroy_menu    = true;
                  _need_warm_reset = true;
                  _worker          = std::make_unique<WifiSetupWorker>();
              }}},
        },
        {
            "Device",
            {{"Brightness",
              [&]() {
                  _destroy_menu = true;
                  _worker       = std::make_unique<BrightnessSetupWorker>();
              }},
             {"Volume",
              [&]() {
                  _destroy_menu = true;
                  _worker       = std::make_unique<VolumeSetupWorker>();
              }},
             {"Timezone",
              [&]() {
                  _destroy_menu = true;
                  _worker       = std::make_unique<TimezoneWorker>();
              }}},
        },
        {
            "AI.Agent",
            {{"General",
              [&]() {
                  _destroy_menu    = true;
                  _need_warm_reset = true;
                  _worker          = std::make_unique<XiaozhiGeneralWorker>();
              }},
             {"Power Saving",
              [&]() {
                  _destroy_menu    = true;
                  _need_warm_reset = true;
                  _worker          = std::make_unique<XiaozhiPowerSavingWorker>();
              }}},
        },
        {
            "Hardware Test",
            {{"Servo",
              [&]() {
                  _destroy_menu = true;
                  _worker       = std::make_unique<ZeroCalibrationWorker>();
              }},
             {"Microphone",
              [&]() {
                  _destroy_menu = true;
                  _worker       = std::make_unique<MicTestWorker>();
              }},
             {"RGB Strip",
              [&]() {
                  _destroy_menu = true;
                  _worker       = std::make_unique<RgbTestWorker>();
              }}},
        },
        {
            "Account",
            {{"Unbind & Reset",
              [&]() {
                  _destroy_menu    = true;
                  _need_warm_reset = true;
                  _worker          = std::make_unique<AccountWorker>();
              }}},
        },
        {
            "Firmware",
            {
                {fmt::format("Version:  {}", common::FirmwareVersion),
                 [&]() {
                     _magic_count++;
                     if (_magic_count >= 10) {
                         _magic_count  = 0;
                         _destroy_menu = true;
                         _worker       = std::make_unique<FwVersionWorker>();
                     }
                 }},
                {"Check for Updates",
                 [&]() {
                     _destroy_menu    = true;
                     _need_warm_reset = true;
                     _worker          = std::make_unique<SystemUpdateWorker>();
                 }},
                //  {"Factory Reset",
                //   [&]() {
                //       _destroy_menu = true;
                //       _worker       = std::make_unique<FactoryResetWorker>();
                //   }}
            },
        },
    };

#ifdef STACKCHAN_COMPANION_BOOT
    for(auto& section:_menu_sections)for(auto& item:section.items) {
        if(item.label=="Brightness" || item.label=="Volume") { item.label+=" (this boot)";continue; }
        const auto feature=item.label;
        item.onClick=[this,feature](){_destroy_menu=true;_worker=std::make_unique<LockedFeatureWorker>(feature);};
    }
#endif
    LvglLockGuard lock;

    _menu_page = std::make_unique<view::SelectMenuPage>(_menu_sections);

    view::create_home_indicator([&]() { close(); });
    view::create_status_bar();
}

void AppSetup::onRunning()
{
    LvglLockGuard lock;

    if (_menu_page) {
        _menu_page->update();
    }

    if (_destroy_menu) {
        _menu_page.reset();
        _destroy_menu = false;
    }

    if (_worker) {
        _worker->update();
        if (_worker->isDone()) {
            _worker.reset();
            _menu_page = std::make_unique<view::SelectMenuPage>(_menu_sections);
        }
    }

    GetStackChan().update();

    view::update_home_indicator();
    view::update_status_bar();
}

void AppSetup::onClose()
{
    mclog::tagInfo(getAppInfo().name, "on close");

    LvglLockGuard lock;

    _menu_sections.clear();
    _menu_page.reset();
    _worker.reset();

    view::destroy_home_indicator();
    view::destroy_status_bar();

    if (_need_warm_reset) {
        GetHAL().requestWarmReboot(6);
    }
}
