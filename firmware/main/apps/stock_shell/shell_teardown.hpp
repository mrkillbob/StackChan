#pragma once
#include <mooncake.h>
namespace stock_shell {
// Mooncake uninstall destroys owners without invoking onClose. Drain live
// close callbacks, detach global UI callbacks, and destroy all UI owners while
// the same recursive LVGL mutex remains held throughout the transition.
template<class UiLock,class ClearGlobals,class DestroyRuntime>
void TearDownShell(mooncake::Mooncake& shell,ClearGlobals clear_globals,DestroyRuntime destroy_runtime){
    UiLock lock;
    for(const auto& app:shell.getAllAppProps()){
        auto state=shell.getAppCurrentState(app.appID);
        if(state==mooncake::AppAbility::StateRunning||state==mooncake::AppAbility::StateGoOpen||state==mooncake::AppAbility::StateGoClose)
            shell.closeApp(app.appID);
    }
    shell.update(); // executes onClose before destroying the live app
    clear_globals(); // also covers apps that were already sleeping
    shell.uninstallAllApps();
    destroy_runtime(); // extension UI owners must use the same lock
}
}
