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
#include <apps/common/common.h>

#ifdef STACKCHAN_COMPANION_BOOT
#include <companion/settings_policy.hpp>
#include <apps/network_setup/network_setup.h>
#include <apps/stock_services/stock_services.h>
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
#ifdef STACKCHAN_COMPANION_BOOT
    companion::SetSettingsScope(companion::SettingsScope::None);
#endif

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
#ifdef STACKCHAN_COMPANION_BOOT
                         _worker.reset();
                         companion::SetSettingsScope(companion::SettingsScope::None);
#endif
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
    // Lightweight settings remain native workers. Radio/hardware/account work
    // opens an explicit preview and transfers to its isolated runtime on Start.
    // Keep the unmodified callbacks above for the original non-companion build.
    using Scope = companion::SettingsScope;
    using Feature = stock_services::Feature;
    for (auto& section : _menu_sections) {
        for (auto& item : section.items) {
            Scope scope = Scope::None;
            std::function<std::unique_ptr<WorkerBase>()> factory;
            if (item.label == "Change Wi-Fi") factory = [] { return network_setup::MakeWorker(); };
            else if (item.label == "Brightness") {
                scope = Scope::Brightness;
                factory = [] { return std::make_unique<BrightnessSetupWorker>(); };
            } else if (item.label == "Volume") {
                scope = Scope::Volume;
                factory = [] { return std::make_unique<VolumeSetupWorker>(); };
            } else if (item.label == "Timezone") {
                scope = Scope::Timezone;
                factory = [] { return std::make_unique<TimezoneWorker>(); };
            } else if (item.label == "General") {
                scope = Scope::AiGeneral;
                factory = [] { return std::make_unique<XiaozhiGeneralWorker>(); };
            } else if (item.label == "Power Saving") {
                scope = Scope::AiPower;
                factory = [] { return std::make_unique<XiaozhiPowerSavingWorker>(); };
            } else if (item.label == "Servo") {
                factory = [] { return stock_services::MakeWorker(Feature::Servo); };
            } else if (item.label == "Microphone") {
                factory = [] { return stock_services::MakeWorker(Feature::Microphone); };
            } else if (item.label == "RGB Strip") {
                factory = [] { return stock_services::MakeWorker(Feature::Rgb); };
            } else if (item.label == "Unbind & Reset") {
                factory = [] { return stock_services::MakeWorker(Feature::Account); };
            } else if (item.label == "Check for Updates") {
                factory = [] { return stock_services::MakeWorker(Feature::FirmwareCheck); };
            }
            // The version row keeps its original ten-tap FwVersionWorker.
            if (!factory) continue;
            item.onClick = [this, scope, factory]() {
                _destroy_menu = true;
                _need_warm_reset = false;
                // Destroy the old worker while its own scope is still active.
                _worker.reset();
                companion::SetSettingsScope(scope);
                _worker = factory();
            };
        }
    }
#endif
#ifdef STACKCHAN_COMPANION_BOOT
    _menu_sections.front().items.push_back({"Welcome Setup", [this](){
        _destroy_menu=true;_worker.reset();companion::SetSettingsScope(companion::SettingsScope::None);
        _worker=stock_services::MakeWorker(stock_services::Feature::Onboarding);
    }});
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
#ifdef STACKCHAN_COMPANION_BOOT
            // Confirmed values are saved by worker destructors above.
            companion::SetSettingsScope(companion::SettingsScope::None);
#endif
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
#ifdef STACKCHAN_COMPANION_BOOT
    companion::SetSettingsScope(companion::SettingsScope::None);
#endif

    view::destroy_home_indicator();
    view::destroy_status_bar();

#ifndef STACKCHAN_COMPANION_BOOT
    if (_need_warm_reset) {
        GetHAL().requestWarmReboot(6);
    }
#endif
}
