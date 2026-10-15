#pragma once
#include <CoreAudio/CoreAudio.h>
#include <CoreFoundation/CoreFoundation.h>
#include <cstring>
namespace companion::directional {
inline constexpr const char* feed="local.mrkillbob.StackChan.hal.feed.v2";
inline constexpr const char* returned="local.mrkillbob.StackChan.hal.return.v2";
using Reader=decltype(&AudioObjectGetPropertyData);
template<class T> bool get(AudioObjectID id,UInt32 selector,T& out,Reader read,UInt32 scope=kAudioObjectPropertyScopeGlobal){
    AudioObjectPropertyAddress address{selector,scope,kAudioObjectPropertyElementMain};UInt32 size=sizeof(out);
    return read(id,&address,0,nullptr,&size,&out)==noErr && size==sizeof(out);
}
inline bool string(AudioObjectID id,UInt32 selector,const char* expected,Reader read){
    CFStringRef value=nullptr;
    const bool ok=get(id,selector,value,read);bool match=false;
    if(ok && value){char text[256]{};match=CFStringGetCString(value,text,sizeof(text),kCFStringEncodingUTF8) && std::strcmp(text,expected)==0;}
    if(value)CFRelease(value);return match;
}
inline AudioDeviceID resolve(const char* uid,Reader read=&AudioObjectGetPropertyData){
    // Exact translation finds hidden devices; ordinary Devices enumeration does not.
    if(!uid || (std::strcmp(uid,feed) && std::strcmp(uid,returned)))return kAudioObjectUnknown;
    CFStringRef qualifier=CFStringCreateWithCString(nullptr,uid,kCFStringEncodingUTF8);
    if(!qualifier)return kAudioObjectUnknown;
    AudioObjectPropertyAddress a{kAudioHardwarePropertyTranslateUIDToDevice,kAudioObjectPropertyScopeGlobal,kAudioObjectPropertyElementMain};
    AudioDeviceID id=kAudioObjectUnknown;UInt32 size=sizeof(id);
    const auto status=read(kAudioObjectSystemObject,&a,sizeof(qualifier),&qualifier,&size,&id);CFRelease(qualifier);
    if(status || size!=sizeof(id) || id==kAudioObjectUnknown)return kAudioObjectUnknown;
    const bool input=std::strcmp(uid,returned)==0;
    UInt32 hidden=0,transport=0,owner=0,ownerClass=0;
    if(!string(id,kAudioDevicePropertyDeviceUID,uid,read) ||
       !get(id,kAudioDevicePropertyIsHidden,hidden,read) || hidden!=1 ||
       !get(id,kAudioDevicePropertyTransportType,transport,read) || transport!=kAudioDeviceTransportTypeVirtual ||
       !get(id,kAudioObjectPropertyOwner,owner,read) || owner==kAudioObjectUnknown ||
       !get(owner,kAudioObjectPropertyClass,ownerClass,read) || ownerClass!=kAudioPlugInClassID ||
       !string(owner,kAudioPlugInPropertyBundleID,"local.mrkillbob.StackChan.DirectionalAudio",read))return kAudioObjectUnknown;
    AudioStreamID stream=kAudioObjectUnknown;
    if(!get(id,kAudioDevicePropertyStreams,stream,read,input?kAudioObjectPropertyScopeInput:kAudioObjectPropertyScopeOutput) || stream==kAudioObjectUnknown)return kAudioObjectUnknown;
    AudioObjectPropertyAddress opposite{kAudioDevicePropertyStreams,input?kAudioObjectPropertyScopeOutput:kAudioObjectPropertyScopeInput,kAudioObjectPropertyElementMain};
    UInt32 empty=sizeof(AudioStreamID);AudioStreamID unexpected=0;
    if(read(id,&opposite,0,nullptr,&empty,&unexpected)!=noErr || empty!=0)return kAudioObjectUnknown;
    AudioStreamBasicDescription physical{};UInt32 direction=2;
    if(!get(stream,kAudioStreamPropertyDirection,direction,read) || direction!=UInt32(input) ||
       !get(stream,kAudioStreamPropertyPhysicalFormat,physical,read) || physical.mSampleRate!=48000 || physical.mChannelsPerFrame!=2 ||
       physical.mFormatID!=kAudioFormatLinearPCM || physical.mFormatFlags!=kAudioFormatFlagsNativeFloatPacked ||
       physical.mBytesPerFrame!=8 || physical.mBytesPerPacket!=8 || physical.mBitsPerChannel!=32 || physical.mFramesPerPacket!=1)return kAudioObjectUnknown;
    return id;
}
}
