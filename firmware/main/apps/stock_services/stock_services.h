#pragma once
#include "policy.h"
#include <mooncake.h>
#include <memory>
namespace setup_workers { class WorkerBase; }

namespace stock_services {
// Factories construct stopped previews. They do not initialize native hardware.
std::unique_ptr<mooncake::AppAbility> MakeApp(Feature feature);
std::unique_ptr<setup_workers::WorkerBase> MakeWorker(Feature feature);
std::optional<Feature> ConsumeRequest();
// An intentional driver reset restores only a preview, never an active service.
std::optional<Feature> ConsumePreviewRequest();
// Caller must first uninstall/destroy Mooncake and close Companion resources.
[[noreturn]] void RunFeature(Feature feature);
}
