#pragma once
#include <string>
namespace companion {
struct AuthorizedGrant { std::string uri, device, credential, request; bool host_audio_qualified=false; };
// Explicit trusted caller only. No network/serial provisioning listener, token issuance or NVS.
bool StageAuthorizedGrant(AuthorizedGrant grant);
[[noreturn]] void RunBoot();
}
