// Companion-only TLS transport. Vendor network implementations are unchanged.
#pragma once
#include "tcp.h"
#include <atomic>
#include <mutex>
#include <esp_tls.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
extern "C" const char* companion_local_ca_pem();
extern "C" const char* companion_local_tls_host();
namespace companion {
class LocalTls final : public Tcp {
public:
    LocalTls();
    ~LocalTls() override;
    bool Connect(const std::string& host,int port) override;
    void Disconnect() override;
    int Send(const std::string& data) override;
    int GetLastError() override;
private:
    esp_tls_t* tls_client_=nullptr;
    EventGroupHandle_t event_group_=nullptr;
    TaskHandle_t receive_task_handle_=nullptr;
    TaskHandle_t owner_task_=nullptr;
    std::atomic<bool> active_{false};
    int last_error_=0;
    std::mutex send_mutex_;
    void ReceiveTask();
};
}
