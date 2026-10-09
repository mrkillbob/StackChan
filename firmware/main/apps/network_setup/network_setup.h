#pragma once
#include <memory>
#include <mooncake.h>
#include "persistence_gate.h"
namespace setup_workers { class WorkerBase; }
namespace network_setup {
// Both entry points open a stopped preview; radios start only on a Start button.
std::unique_ptr<mooncake::AppAbility> MakeApp();
std::unique_ptr<setup_workers::WorkerBase> MakeWorker();
// Open the stopped preview after an intentional shared-driver reset, never start it.
bool ConsumePreviewRequest();
}
