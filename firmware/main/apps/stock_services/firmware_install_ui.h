#pragma once
#include <string>
namespace stock_services {
bool OfferFirmwareInstall(const std::string& url,const std::string& version={});
bool PrepareOriginalFirmwareWifi();
}
