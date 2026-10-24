#pragma once
#include "stop_sequence.hpp"
#include <functional>
namespace runtime_lifecycle {
StopSequence& Session();
bool ConsumeStateReset();
bool Begin();void RequestHome();bool HomeRequested();bool Active();
bool DeferNativeClose();void FinalizeNativeUi(bool);
// Wait-free notifications from the Application and HAL update worker.
void ApplicationStarted();void ApplicationExited();bool ApplicationJoined();
void ActivationStarted();void ActivationExited();bool ActivationJoined();
void UpdateStarted();void UpdateExited();bool UpdateJoined();
// Claim before task creation; publish exit only after permission/UI/request
// cleanup. Home retains session/network ownership until this worker joins.
bool InstallerStarted();void InstallerExited();bool InstallerJoined();
// Serialize final boot selection or immediate reboot against RequestHome.
// The callback must be short and must not call RequestHome or wait for UI.
bool CommitFirmwareInstall(const std::function<bool()>& commit);
bool WaitCancelled(uint32_t milliseconds);
}
