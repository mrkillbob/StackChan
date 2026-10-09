#pragma once
#include "provisioning_ack.hpp"
#include <algorithm>
#include <array>
#include <string>
namespace companion {
struct NumericUsbRequest {std::string request;uint32_t boot_nonce=0;};
enum class NumericUsbRead {Waiting,Accepted,Rejected};
inline bool DecodeNumericUsb(const uint8_t* p,size_t n,uint32_t boot,NumericUsbRequest& out){
 out={};if(!p||n!=25||!boot||p[0]!='S'||p[1]!='C'||p[2]!='N'||p[3]!='P'||p[4]!=1)return false;
 uint32_t value=0;for(unsigned i=21;i<25;++i)value=(value<<8)|p[i];if(value!=boot)return false;
 bool nonzero=false;const char* hex="0123456789abcdef";for(unsigned i=5;i<21;++i){nonzero|=p[i]!=0;out.request.push_back(hex[p[i]>>4]);out.request.push_back(hex[p[i]&15]);}
 if(!nonzero){out={};return false;}out.boot_nonce=boot;return true;
}
// Serialized CPU0 owner. SCNP contains only a request UUID and boot nonce.
template<class Driver> class NumericUsbOwner {
 Driver& driver_;bool owned_=false,active_=false;uint32_t boot_=0;uint64_t deadline_=0;std::array<uint8_t,26> bytes_{};size_t used_=0;
 bool Emit(const AckFrame& a){return owned_&&driver_.Installed()&&driver_.OnOwnerCore()&&driver_.WriteAck(a.data(),a.size())==static_cast<int>(a.size())&&driver_.FlushAck();}
public:
 explicit NumericUsbOwner(Driver& d):driver_(d){}~NumericUsbOwner(){Close();}
 NumericUsbOwner(const NumericUsbOwner&)=delete;NumericUsbOwner& operator=(const NumericUsbOwner&)=delete;
 bool Active()const{return owned_&&active_;}
 bool Close(){active_=false;bytes_.fill(0);used_=0;deadline_=0;if(!owned_)return true;if(!driver_.Installed()){owned_=false;return true;}if(!driver_.OnOwnerCore()||!driver_.Uninstall())return false;owned_=false;return true;}
 bool Open(uint64_t now,uint32_t boot,uint32_t reason){
  if(!Close()||!boot||reason>64||now>UINT64_MAX-60000||!driver_.OnOwnerCore()||driver_.Installed())return false;
  if(!driver_.Install()){return false;}
  owned_=true;
  std::array<uint8_t,128> stale{};bool found=false;
  for(unsigned i=0;i<8;++i){int n=driver_.Read(stale.data(),stale.size());stale.fill(0);if(n<0||n>128){Close();return false;}if(!n)break;found=true;}
  if(found){Emit(EncodeStatusAck(AckStatus::Rejected));Close();return false;}
  boot_=boot;deadline_=now+60000;active_=true;if(!Emit(EncodeReadyAck(boot,reason))){Close();return false;}return true;
 }
 NumericUsbRead Poll(uint64_t now,NumericUsbRequest& out){
  out={};if(!Active())return NumericUsbRead::Rejected;
  if(!driver_.Installed()||!driver_.OnOwnerCore()){Close();return NumericUsbRead::Rejected;}
  if(now>=deadline_){Emit(EncodeStatusAck(AckStatus::Expired));Close();return NumericUsbRead::Rejected;}
  int n=driver_.Read(bytes_.data()+used_,bytes_.size()-used_);
  if(n<0||static_cast<size_t>(n)>bytes_.size()-used_){Close();return NumericUsbRead::Rejected;}used_+=n;
  if(used_<25)return NumericUsbRead::Waiting;
  bool valid=DecodeNumericUsb(bytes_.data(),used_,boot_,out);
  if(!valid){Emit(EncodeStatusAck(AckStatus::Rejected));}
  bool closed=Close();if(!valid||!closed){out={};return NumericUsbRead::Rejected;}return NumericUsbRead::Accepted;
 }
};
}
