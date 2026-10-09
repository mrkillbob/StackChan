#pragma once
#include <mooncake.h>
#include <memory>
namespace stock_agent {
std::unique_ptr<mooncake::AppAbility> MakeApp();
// Run only after this consumes an explicit physical Start and Mooncake is destroyed.
bool ConsumeStartRequest();
// One-shot stopped preview, never an active session, on intentional software reset only.
bool ConsumePreviewRequest();
[[noreturn]] void RunStockAgent();
}
