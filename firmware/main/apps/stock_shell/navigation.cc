#include "navigation.h"
#include "menu_policy.hpp"
#include <esp_attr.h>
#include <esp_system.h>
#include <atomic>
#include <cstdint>
namespace stock_shell {
namespace {
std::atomic<bool> setup_context{false};
constexpr uint32_t kMagic=0x53455455;
struct ReturnCookie {uint32_t magic,inverse;};
RTC_NOINIT_ATTR ReturnCookie setup_return;
}
void SetSetupContext(bool active){setup_context=active;}
void SaveSetupReturnForReset(){
 auto feature=stock_services::CurrentFeature();
 bool setup=setup_context.load()||(feature&&IsSetupFeature(*feature));
 setup_return=setup?ReturnCookie{kMagic,~kMagic}:ReturnCookie{};
}
bool ConsumeSetupReturn(){
 auto cookie=setup_return;setup_return={};
 return esp_reset_reason()==ESP_RST_SW&&cookie.magic==kMagic&&cookie.inverse==~kMagic;
}
}
