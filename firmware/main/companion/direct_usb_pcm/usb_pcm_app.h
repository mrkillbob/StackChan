#pragma once
#include <mooncake.h>
#include <memory>
#include <functional>
namespace direct_usb_pcm {std::unique_ptr<mooncake::AppAbility> MakeApp(std::function<void()> returned = {});}
