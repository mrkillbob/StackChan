#include "companion/diagnostic_usb_export.hpp"
#include <cassert>
#include <array>
struct Driver {
 bool core=true,installed=false,install_ok=true,close_ok=true,flush_ok=true;int written=-2;
 unsigned installs=0,closes=0,writes=0,flushes=0;
 bool OnOwnerCore(){return core;}bool Installed(){return installed;}
 bool InstallDiagnostic(){++installs;if(install_ok)installed=true;return install_ok;}
 bool Uninstall(){++closes;if(close_ok)installed=false;return close_ok;}
 int WriteAck(const uint8_t*,size_t n){++writes;return written==-2?static_cast<int>(n):written;}
 bool FlushAck(){++flushes;return flush_ok;}
};
int main(){std::array<uint8_t,176> frame{};
 {Driver d;companion::DiagnosticUsbExport<Driver> export_(d);assert(!export_.Export(false,frame.data(),frame.size()));assert(d.installs==0&&d.writes==0);}
 {Driver d;d.core=false;companion::DiagnosticUsbExport<Driver> export_(d);assert(!export_.Export(true,frame.data(),frame.size()));assert(d.installs==0&&d.closes==0);}
 {Driver d;d.installed=true;companion::DiagnosticUsbExport<Driver> export_(d);assert(!export_.Export(true,frame.data(),frame.size()));assert(d.closes==0&&d.installs==0&&d.installed);}
 {Driver d;d.install_ok=false;companion::DiagnosticUsbExport<Driver> export_(d);assert(!export_.Export(true,frame.data(),frame.size()));assert(d.writes==0&&!export_.OwnsDriver());}
 for(int n:{0,1,-1,175,177}){Driver d;d.written=n;companion::DiagnosticUsbExport<Driver> export_(d);assert(!export_.Export(true,frame.data(),frame.size()));assert(d.writes==1&&d.flushes==0&&d.closes==1&&!d.installed);}
 {Driver d;d.flush_ok=false;companion::DiagnosticUsbExport<Driver> export_(d);assert(!export_.Export(true,frame.data(),frame.size()));assert(d.writes==1&&d.flushes==1&&d.closes==1&&!d.installed);}
 {Driver d;d.close_ok=false;companion::DiagnosticUsbExport<Driver> export_(d);assert(!export_.Export(true,frame.data(),frame.size()));assert(export_.OwnsDriver()&&d.installed);d.core=false;assert(!export_.Close()&&d.closes==1);d.core=true;d.close_ok=true;assert(export_.Close()&&!export_.OwnsDriver());}
 {Driver d;companion::DiagnosticUsbExport<Driver> export_(d);assert(export_.Export(true,frame.data(),frame.size()));assert(d.writes==1&&d.flushes==1&&d.closes==1&&!d.installed&&!export_.OwnsDriver());assert(!export_.Export(true,frame.data(),257));assert(!export_.Export(true,nullptr,176));}
}
