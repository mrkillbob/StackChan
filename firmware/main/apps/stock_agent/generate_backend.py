#!/usr/bin/env python3
"""Generate reviewed variants from exact pinned vendor sources; no fetched file edits."""
import argparse
import hashlib
from pathlib import Path

PINNED = {
    'application.cc': '75e4955346138c14e5bb72da9319d4bce5d54b9f737eba3cdbe63eb487cb58c5',
    'ota.cc': '7b046477f51deace32169f177c69c368cb6a94294305910f62078410f43893cf',
    'boards/common/wifi_board.cc': 'df4afcfdd13bae8ebdf0eb6b8d31f7a334aefb01b660f95bf7d4ababecbf789c',
    'wifi_manager.cc': '3346377f3595774d8a8d07b259f4860a9aabaf323e39fba6c1cac598e44d5d20',
    'wifi_configuration_ap.cc': '05e2d1fe46ce57de433078f287accfc06df771c326a23b9eac894d6657a465c3',
    'ssid_manager.cc': 'e3441e0fc84012b539bd08b680cdfbfdaca5eb55af55423400268d0a18d5daca',
}

def pinned(path, name):
    data = path.read_bytes()
    if hashlib.sha256(data).hexdigest() != PINNED[name]:
        raise SystemExit(f'Unreviewed vendor source: {name}; re-review adapter before updating pin')
    return data.decode()

def replace_once(source, before, after):
    if source.count(before) != 1:
        raise SystemExit('Vendor adapter anchor did not match exactly once')
    return source.replace(before, after, 1)

def replace_region(source, begin, end, replacement):
    start = source.index(begin)
    finish = source.index(end, start)
    return source[:start] + replacement + '\n\n' + source[finish:]

def generate(xiaozhi, wifi, output):
    output.mkdir(parents=True, exist_ok=True)
    source = pinned(xiaozhi / 'application.cc', 'application.cc')
    source = replace_once(source, '    mcp_server.AddUserOnlyTools();',
        '    // Update/reboot/asset-download user tools are excluded in the selectable-agent build.')
    source = replace_region(source, 'void Application::CheckAssetsVersion() {',
        'void Application::CheckNewVersion() {', '''void Application::CheckAssetsVersion() {
    if (assets_version_checked_) return;
    assets_version_checked_ = true;
    auto& assets = Assets::GetInstance();
    // Apply local assets and voice models only. Never consume saved download_url.
    if (assets.partition_valid()) assets.Apply();
}''')
    source = replace_once(source, '''        if (ota_->HasNewVersion()) {
            if (UpgradeFirmware(ota_->GetFirmwareUrl(), ota_->GetFirmwareVersion())) {
                return; // This line will never be reached after reboot
            }
            // If upgrade failed, continue to normal operation
        }''', '        // Activation metadata is retained; firmware downloads are never accepted.')
    source = replace_region(source,
        'bool Application::UpgradeFirmware(const std::string& url, const std::string& version) {',
        'void Application::WakeWordInvoke(const std::string& wake_word) {',
        '''bool Application::UpgradeFirmware(const std::string& url, const std::string& version) {
    (void)url; (void)version;
    return false; // Explicit firmware installation belongs to the host workflow.
}''')
    (output / 'stock_application.cc').write_text(source)

    source = pinned(xiaozhi / 'ota.cc', 'ota.cc')
    source = replace_once(source,
        '        ESP_LOGI(TAG, "Setup HTTP, User-Agent: %s, Serial-Number: %s", user_agent.c_str(), serial_number_.c_str());',
        '        // Serial identity is sent only after explicit Start and never logged.')
    source = replace_once(source,
        '        ESP_LOGE(TAG, "Failed to activate, code: %d, body: %s", status_code, http->ReadAll().c_str());',
        '        ESP_LOGE(TAG, "Failed to activate, code: %d", status_code);')
    source = replace_region(source,
        'bool Ota::Upgrade(const std::string& firmware_url, std::function<void(int progress, size_t speed)> callback) {',
        'bool Ota::StartUpgrade(std::function<void(int progress, size_t speed)> callback) {',
        '''bool Ota::Upgrade(const std::string& firmware_url, std::function<void(int progress, size_t speed)> callback) {
    (void)firmware_url; (void)callback;
    return false; // No OTA firmware download or flash writes in this variant.
}''')
    (output / 'stock_ota.cc').write_text(source)

    source = pinned(xiaozhi / 'boards/common/wifi_board.cc', 'boards/common/wifi_board.cc')
    source = replace_once(source, '    wifi_manager.Initialize(config);', '''    if (!wifi_manager.Initialize(config)) {
        Application::GetInstance().Alert("Stock AI", "Wi-Fi failed. Stop / Home to return.", "triangle_exclamation");
        return;
    }''')
    (output / 'stock_wifi_board.cc').write_text(source)

    source = pinned(wifi / 'wifi_manager.cc', 'wifi_manager.cc')
    source = replace_once(source, '''    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing NVS...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }''', '    // Preserve pairing, calibration and Wi-Fi on every initialization error.')
    (output / 'stock_wifi_manager.cc').write_text(source)

    source = pinned(wifi / 'wifi_configuration_ap.cc', 'wifi_configuration_ap.cc')
    begin = '    // Register the /advanced/submit URI'
    end = '    ESP_ERROR_CHECK(httpd_register_uri_handler(server_, &advanced_submit));'
    first = source.index(begin)
    last = source.index(end, first) + len(end)
    source = source[:first] + r'''    // Persistent advanced changes are unavailable in a session-only adapter.
    httpd_uri_t advanced_submit = {
        .uri = "/advanced/submit",
        .method = HTTP_POST,
        .handler = [](httpd_req_t *req) -> esp_err_t {
            httpd_resp_set_status(req, "403 Forbidden");
            httpd_resp_set_type(req, "application/json");
            httpd_resp_set_hdr(req, "Connection", "close");
            httpd_resp_send(req, "{\"success\":false,\"error\":\"Persistent settings are preserved; use normal Wi-Fi setup for this session\"}", HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        },
        .user_ctx = this
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server_, &advanced_submit));''' + source[last:]
    source = replace_once(source,
        '            ESP_LOGI(TAG, "SmartConfig SSID: %s, Password: %s", ssid, password);',
        '            // Never log Wi-Fi credentials.')
    (output / 'stock_wifi_configuration_ap.cc').write_text(source)

    source = pinned(wifi / 'ssid_manager.cc', 'ssid_manager.cc')
    source = replace_region(source, 'void SsidManager::SaveToNvs() {',
        'void SsidManager::AddSsid(const std::string& ssid, const std::string& password) {',
        '''void SsidManager::SaveToNvs() {
    // Phone provisioning changes only this session's SSID list. Existing NVS
    // credentials and other namespaces remain unchanged across Stop / Home.
}''')
    (output / 'stock_ssid_manager.cc').write_text(source)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--xiaozhi', type=Path, required=True)
    parser.add_argument('--wifi', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    generate(args.xiaozhi, args.wifi, args.output)
