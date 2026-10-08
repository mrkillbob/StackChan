#pragma once
#include "codec_tasks.h"
#include <network_interface.h>
#include "bounded_web_socket.h"
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
namespace companion {
// Explicit, volatile connection grant. No NVS/token issuance or auto-connect.
// Endpoint is WSS with vendor ESP TLS certificate verification, no insecure mode.
class CompanionSocket {
public:
    explicit CompanionSocket(CodecTasks& tasks):tasks_(tasks){}
    bool Connect(NetworkInterface& network,const std::string& uri,const std::string& device_id,const std::string& credential,const std::string& request_id);
    bool PhysicalConsent();
    bool SendAudio(const uint8_t* data,std::size_t bytes);
    void Tick(); void Close();
private:
    CodecTasks& tasks_;std::shared_ptr<BoundedWebSocket> socket_;std::mutex mutex_;
    std::atomic<uint32_t> epoch_{0};std::atomic<bool> consent_{false};
    uint64_t last_heartbeat_ms_=0;std::string request_id_;
    bool Text(const std::string& message);void Data(const char* data,std::size_t bytes,bool binary);
};
}
