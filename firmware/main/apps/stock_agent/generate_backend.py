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
    source = '#include <apps/network_setup/wifi_credentials.hpp>\n#include <apps/network_setup/persistence_gate.h>\n' + source
    source = replace_once(source, '            this_->Save(ssid_str, password_str);',
        '            this_->Save(ssid_str, password_str);\n            if (network_setup::CredentialPersistenceAllowed() && !network_setup::CredentialSaveSucceeded()) { cJSON_Delete(json); httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Wi-Fi connected but could not be saved"); return ESP_OK; }')

    source = replace_once(source, '    strlcpy((char *)wifi_config.sta.ssid, ssid.c_str(), 32);\n    strlcpy((char *)wifi_config.sta.password, password.c_str(), 64);',
        '    if (!network_setup::Valid({ssid, password})) return false;\n    memcpy(wifi_config.sta.ssid, ssid.data(), ssid.size());\n    memcpy(wifi_config.sta.password, password.data(), password.size());')
    source = replace_once(source, '            if (!this_->ConnectToWifi(ssid_str, password_str)) {',
        '            if (!network_setup::Valid({ssid_str, password_str}) || !this_->ConnectToWifi(ssid_str, password_str)) {')
    source = replace_once(source, '            std::string password_str = "";',
        '            if (!cJSON_IsString(password_item) || !password_item->valuestring || strlen(password_item->valuestring) > 64) { cJSON_Delete(json); httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid password"); return ESP_OK; }\n            std::string password_str = "";')
    source = replace_once(source, '    ESP_LOGI(TAG, "Save SSID %s %d", ssid.c_str(), ssid.length());',
        '    // No credential or network-name logging.')

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
    source = replace_once(source, '            self->Save(ssid, password);',
        '            // SmartConfig credentials are not saved before a verified connection.')
    # Suppress network names in logs; local setup UI still receives the scan list.
    lines = source.splitlines(keepends=True)
    clean = []; index = 0
    while index < len(lines):
        line = lines[index]
        if 'ESP_LOG' in line and ('%s' in line) and ('SSID' in line or 'WiFi' in line):
            while ');' not in lines[index]: index += 1
            clean.append('    // Wi-Fi name omitted from logs.\n')
        else: clean.append(line)
        index += 1
    source = ''.join(clean)
    (output / 'stock_wifi_configuration_ap.cc').write_text(source)

    source = pinned(wifi / 'ssid_manager.cc', 'ssid_manager.cc')
    source = '#include <apps/network_setup/persistence_gate.h>\n#include <apps/network_setup/wifi_credentials.hpp>\n' + source
    source = replace_once(source, '    ssid_list_.clear();\n\n    // Load ssid', """    ssid_list_.clear();
    nvs_handle_t saved = 0;
    if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &saved) == ESP_OK) {
        size_t size = 0;
        if (nvs_get_blob(saved, "config_v1", nullptr, &size) == ESP_OK && size <= 1024) {
            std::vector<uint8_t> blob(size);
            std::vector<network_setup::Credential> credentials;
            if (nvs_get_blob(saved, "config_v1", blob.data(), &size) == ESP_OK &&
                network_setup::Decode(blob.data(), size, credentials)) {
                for (const auto& item : credentials) ssid_list_.push_back({item.ssid, item.password});
                nvs_close(saved);
                return;
            }
        }
        nvs_close(saved);
    }
    // Legacy keys remain untouched and serve as a fallback after a damaged blob.
    // Load ssid""")
    source = replace_once(source, '        ssid_list_.push_back({ssid, password});',
        '        if (network_setup::Valid({ssid, password})) ssid_list_.push_back({ssid, password});')
    source = replace_region(source, 'void SsidManager::SaveToNvs() {',
        'void SsidManager::AddSsid(const std::string& ssid, const std::string& password) {',
        """void SsidManager::SaveToNvs() {
    if (!network_setup::CredentialPersistenceAllowed()) return;
    std::vector<network_setup::Credential> credentials;
    for (const auto& item : ssid_list_) credentials.push_back({item.ssid, item.password});
    std::vector<uint8_t> blob;
    if (!network_setup::Encode(credentials, blob)) {
        network_setup::NotifyCredentialsSaved(false); return;
    }
    nvs_handle_t handle = 0;
    auto result = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (result == ESP_OK) {
        result = nvs_set_blob(handle, "config_v1", blob.data(), blob.size());
        if (result == ESP_OK) result = nvs_commit(handle);
        nvs_close(handle);
    }
    // One atomic NVS blob update; legacy Wi-Fi, calibration and account keys stay intact.
    network_setup::NotifyCredentialsSaved(result == ESP_OK);
}""")
    source = replace_region(source, 'void SsidManager::AddSsid(const std::string& ssid, const std::string& password) {',
        'void SsidManager::RemoveSsid(int index) {', """void SsidManager::AddSsid(const std::string& ssid, const std::string& password) {
    if (!network_setup::Valid({ssid, password})) { network_setup::NotifyCredentialsSaved(false); return; }
    for (auto iter = ssid_list_.begin(); iter != ssid_list_.end(); ++iter) {
        if (iter->ssid == ssid) { ssid_list_.erase(iter); break; }
    }
    if (ssid_list_.size() >= MAX_WIFI_SSID_COUNT) ssid_list_.pop_back();
    ssid_list_.insert(ssid_list_.begin(), {ssid, password});
    SaveToNvs();
}""")
    (output / 'stock_ssid_manager.cc').write_text(source)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--xiaozhi', type=Path, required=True)
    parser.add_argument('--wifi', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    generate(args.xiaozhi, args.wifi, args.output)
