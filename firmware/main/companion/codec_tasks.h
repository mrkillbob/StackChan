#pragma once
#include "audio_session.hpp"
#include "audio_wire.hpp"
#include <audio/audio_codec.h>
#include <esp_ae_rate_cvt.h>
#include <freertos/task.h>
#include <atomic>
#include <deque>
#include <functional>
#include <mutex>
#include <vector>
namespace companion {
// Exclusive BSP codec owner. No constructor IO, NVS, network, camera or motion.
// Retain this object until Stop() returns true; no forced task deletion.
class CodecTasks {
public:
    using Send=std::function<bool(const uint8_t*,std::size_t)>;
    using Stopped=std::function<void(uint32_t,bool)>;
    bool IsActive() const { return enabled_.load() && !done_.load(); }
    bool IsStopped() const { return done_.load(); }
    ~CodecTasks();
    bool Start(AudioCodec* codec,Send send,Stopped stopped);
    bool ResetStoppedConnection();
    bool Stop(uint32_t timeout_ms=2000);
    bool Begin(uint32_t epoch);
    bool Acknowledge(uint32_t epoch);
    bool Consent(uint32_t epoch,bool physical);
    bool Heartbeat(uint32_t epoch);
    bool Playing(uint32_t epoch,bool playing);
    void Interrupt(uint32_t epoch);
    bool Receive(const uint8_t* wire,std::size_t bytes);
private:
    struct Packet { uint32_t epoch; std::vector<uint8_t> payload; };
    AudioSession session_; std::mutex mutex_; std::deque<Packet> incoming_;
    std::atomic<uint32_t> epoch_{0}; std::atomic<bool> enabled_{false},playing_{false},playback_known_{false},quit_{false},done_{true};
    AudioCodec* codec_=nullptr; Send send_; Stopped stopped_; TaskHandle_t task_=nullptr;
    void* encoder_=nullptr; void* decoder_=nullptr; esp_ae_rate_cvt_handle_t resampler_=nullptr;
    std::vector<int16_t> pending_; uint32_t mic_sequence_=0;
    bool OpenCodecs(); void CloseCodecs(); void Run(); bool Current(uint32_t epoch);
};
}
