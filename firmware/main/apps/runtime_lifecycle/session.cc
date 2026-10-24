#include "session.h"
#include "cleanup.h"
#include <companion/station_authority.hpp>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <mutex>
namespace runtime_lifecycle {
namespace {StopSequence session;std::mutex installer_mutex;std::atomic<bool> reset_state{false},finalize{false},app{false},activation{false},update{false},installer{false};}
StopSequence& Session(){return session;}
bool Begin(){
 std::lock_guard<std::mutex> lock(installer_mutex);
 if(app||activation||update||installer||session.Open()||!VendorNetworkAvailable())return false;
 auto& authority=companion::network::Authority();
 if(!authority.Acquire(companion::network::Owner::Vendor))return false;
 finalize=false;const bool ok=session.Begin();reset_state=ok;
 if(!ok){authority.RequestStop(companion::network::Owner::Vendor);authority.FinishStop(companion::network::Owner::Vendor,true);} // no SDK work occurred
 return ok;
}
bool ConsumeStateReset(){return reset_state.exchange(false)&&session.Open()&&!session.Stopping();}
void RequestHome(){std::lock_guard<std::mutex> lock(installer_mutex);session.RequestStop();companion::network::Authority().RequestStop(companion::network::Owner::Vendor);}
bool HomeRequested(){return session.Stopping();}
bool Active(){return session.Open();}
bool DeferNativeClose(){if(!Active()||finalize)return false;RequestHome();return true;}
void FinalizeNativeUi(bool value){finalize=value;}
void ApplicationStarted(){app=true;}void ApplicationExited(){app=false;}bool ApplicationJoined(){return !app.load();}
void ActivationStarted(){activation=true;}void ActivationExited(){activation=false;}bool ActivationJoined(){return !activation.load();}
void UpdateStarted(){update=true;}void UpdateExited(){update=false;}bool UpdateJoined(){return !update.load();}
bool InstallerStarted(){std::lock_guard<std::mutex> lock(installer_mutex);if(!session.Accepts(session.Generation())||installer)return false;installer=true;return true;}
void InstallerExited(){installer=false;}bool InstallerJoined(){return !installer.load();}
bool CommitFirmwareInstall(const std::function<bool()>& commit){
 std::lock_guard<std::mutex> lock(installer_mutex);
 return session.Accepts(session.Generation())&&commit();
}
bool WaitCancelled(uint32_t ms){for(uint32_t elapsed=0;elapsed<ms;elapsed+=20){if(HomeRequested())return true;vTaskDelay(pdMS_TO_TICKS(ms-elapsed<20?ms-elapsed:20));}return HomeRequested();}
}
