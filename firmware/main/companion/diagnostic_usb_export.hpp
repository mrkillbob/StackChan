#pragma once
#include <cstddef>
#include <cstdint>
namespace companion {
// Explicit diagnostics only: no receive, Ready, Pair or audio admission.
// One serialized CPU0 caller owns its SDK interrupt. Foreign drivers are untouched.
template<class Driver> class DiagnosticUsbExport {
    Driver& driver_; bool owned_=false;
public:
    explicit DiagnosticUsbExport(Driver& driver):driver_(driver){}
    bool Close(){
        if(!owned_)return true;
        if(!driver_.Installed()){owned_=false;return true;}
        if(!driver_.OnOwnerCore()||!driver_.Uninstall())return false;
        owned_=false;return true;
    }
    bool Export(bool fully_stopped,const uint8_t* frame,std::size_t bytes){
        if(!fully_stopped||!frame||!bytes||bytes>256||!driver_.OnOwnerCore())return false;
        if(!Close()||driver_.Installed()||!driver_.Install())return false;
        owned_=true;
        // One bounded SDK write/flush; no partial-write retry.
        bool delivered=driver_.WriteAck(frame,bytes)==static_cast<int>(bytes);
        if(delivered)delivered=driver_.FlushAck();
        bool closed=Close();return delivered&&closed;
    }
    bool OwnsDriver()const{return owned_;}
};
}
