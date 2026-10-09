#pragma once
#include <atomic>
#include <string_view>
namespace companion {
enum class SettingsScope { None, Brightness, Volume, Timezone, AiGeneral, AiPower, Calibration, Account, RemoteService };
inline std::atomic<SettingsScope> settings_scope{SettingsScope::None};
inline void SetSettingsScope(SettingsScope scope){settings_scope=scope;}
inline SettingsScope CurrentSettingsScope(){return settings_scope.load();}
inline bool SettingsWriteAllowed(std::string_view ns,std::string_view key){
 switch(CurrentSettingsScope()){
 case SettingsScope::Brightness:return ns=="display"&&key=="brightness";
 case SettingsScope::Volume:return ns=="audio"&&key=="output_volume";
 case SettingsScope::Timezone:return ns=="system"&&key=="tz";
 case SettingsScope::AiGeneral:return (ns=="xiaozhi"&&(key=="idle_lv"||key=="boot_ai"||key=="idle_sec"||key=="ext_pwr"))||(ns=="companion_boot"&&key=="stock_optin");
 case SettingsScope::AiPower:return ns=="xiaozhi"&&(key=="idle_sec"||key=="ext_pwr"||key=="idle_lv"||key=="boot_ai");
 case SettingsScope::Calibration:return ns=="servo"&&(key=="zero_pos_1"||key=="zero_pos_2");
 case SettingsScope::Account:return ns=="account";
 case SettingsScope::RemoteService:return ns=="account"||ns=="stackchan";
 default:return false;
 }
}
inline bool SettingsNamespaceAllowed(std::string_view ns){
 for(auto key:{"brightness","output_volume","tz","idle_lv","idle_sec","zero_pos_1","username","device_name","stock_optin"})if(SettingsWriteAllowed(ns,key))return true;
 return false;
}
}
