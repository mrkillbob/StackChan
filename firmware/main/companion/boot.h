#pragma once
#include <string>
#include <lvgl.h>
namespace companion {
struct AuthorizedGrant { std::string uri, device, credential, request; bool host_audio_qualified=false; };
// Explicit trusted caller only. No network/serial provisioning listener, token issuance or NVS.
bool StageAuthorizedGrant(AuthorizedGrant grant);
void OpenView(lv_obj_t* parent);
void TickView();
bool CloseView(); // false: retain app/UI while codec worker stops
[[noreturn]] void RunBoot();
}
