#include "diagnostic_state.hpp"
#include <esp_attr.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
namespace companion::diagnostic {
namespace {
State state;RTC_NOINIT_ATTR Retained retained;std::atomic<uint32_t> saving{0};
uint32_t Now(){return uint32_t(esp_timer_get_time()/1000);}
void Save(){uint32_t free=0;if(!saving.compare_exchange_strong(free,1))return;retained=Seal(state.Current());saving.store(0,std::memory_order_release);}
}
void Init(uint32_t nonce,uint32_t reset){state.Init(nonce,reset,retained);Save();}
uint32_t BootNonce(){return state.BootNonce();}
void Stage(Task task,StageCode code){state.Mark(task,code,Now());Save();}
void Sample(Task task){state.Sample(task,uint32_t(uxTaskGetStackHighWaterMark(nullptr)));Save();}
void Fault(Task task,FaultCode code,int32_t detail){state.Fail(task,code,detail,Now());Sample(task);}
void CountTick(){state.Tick();}void CountHeartbeat(bool success){state.Heartbeat(success);}
Frame Snapshot(){Save();return state.Snapshot(Now());}
}
