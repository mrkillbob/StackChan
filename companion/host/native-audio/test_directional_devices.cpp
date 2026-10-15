#include "directional_devices.hpp"
#include <cassert>
#include <initializer_list>
using namespace companion::directional;
enum Fault { none,badOwner,badHidden,badDirection,badRate,oppositeStream,badUID,badSize,missing } fault=none;
unsigned translations=0;
OSStatus mockRead(AudioObjectID id,const AudioObjectPropertyAddress* a,UInt32 qn,const void* q,UInt32* size,void* data){
    auto copy=[&](auto value)->OSStatus {if(*size<sizeof(value))return kAudioHardwareBadPropertySizeError;*size=sizeof(value);std::memcpy(data,&value,*size);return noErr;};
    auto text=[&](CFStringRef s){CFRetain(s);return copy(s);};
    if(a->mSelector==kAudioHardwarePropertyTranslateUIDToDevice){
        ++translations;assert(id==kAudioObjectSystemObject && qn==sizeof(CFStringRef));
        CFStringRef uid;std::memcpy(&uid,q,sizeof(uid));
        return copy(AudioObjectID(fault==missing?0:(CFEqual(uid,CFSTR("local.mrkillbob.StackChan.hal.feed.v2"))?6:8)));
    }
    const bool input=id==8;const bool stream=id==7 || id==9;
    switch(a->mSelector){
        case kAudioDevicePropertyDeviceUID:return text(fault==badUID?CFSTR("wrong"):(input?CFSTR("local.mrkillbob.StackChan.hal.return.v2"):CFSTR("local.mrkillbob.StackChan.hal.feed.v2")));
        case kAudioDevicePropertyIsHidden:return copy(UInt32(fault==badHidden?0:1));
        case kAudioDevicePropertyTransportType:return copy(UInt32(kAudioDeviceTransportTypeVirtual));
        case kAudioObjectPropertyOwner:return copy(UInt32(1));
        case kAudioObjectPropertyClass:return copy(UInt32(kAudioPlugInClassID));
        case kAudioPlugInPropertyBundleID:return text(fault==badOwner?CFSTR("other.driver"):CFSTR("local.mrkillbob.StackChan.DirectionalAudio"));
        case kAudioDevicePropertyStreams:
            if(a->mScope==(input?kAudioObjectPropertyScopeInput:kAudioObjectPropertyScopeOutput))return copy(UInt32(id+1));
            *size=fault==oppositeStream?sizeof(AudioStreamID):0;return noErr;
        case kAudioStreamPropertyDirection:assert(stream);return copy(UInt32(fault==badDirection?!(id==9):(id==9)));
        case kAudioStreamPropertyPhysicalFormat:{
            assert(stream);AudioStreamBasicDescription f{};f.mSampleRate=fault==badRate?44100:48000;
            f.mFormatID=kAudioFormatLinearPCM;f.mFormatFlags=kAudioFormatFlagsNativeFloatPacked;
            f.mBytesPerPacket=f.mBytesPerFrame=8;f.mFramesPerPacket=1;f.mChannelsPerFrame=2;f.mBitsPerChannel=32;
            const auto result=copy(f);if(fault==badSize)--*size;return result;
        }
        default:assert(false);return kAudioHardwareUnknownPropertyError;
    }
}
int main(){
    assert(resolve(feed,mockRead)==6);assert(resolve(returned,mockRead)==8);assert(translations==2);
    assert(resolve("Pro Tools Audio Bridge 2-A_UID",mockRead)==0);assert(resolve(nullptr,mockRead)==0);assert(translations==2);
    for(auto f:{badOwner,badHidden,badDirection,badRate,oppositeStream,badUID,badSize,missing}){
        fault=f;assert(resolve(feed,mockRead)==0);assert(resolve(returned,mockRead)==0);
    }
}
