// SPDX-License-Identifier: Apache-2.0
#include "api.hpp"
#include <new>
#include <cstdint>
namespace {
using voice_prep::Protocol;
Protocol* Construct(void* storage,size_t bytes){
 if(!storage||bytes<sizeof(Protocol)||reinterpret_cast<uintptr_t>(storage)%alignof(Protocol))return nullptr;
 return ::new(storage) Protocol();
}
void Destroy(Protocol* p){if(p)p->~Protocol();}
constexpr voice_prep::Api api{1,sizeof(Protocol),alignof(Protocol),Construct,Destroy,
 &Protocol::Status,&Protocol::Pending,&Protocol::Begin,&Protocol::Sent,&Protocol::Head,
 &Protocol::Media,&Protocol::Hold,&Protocol::Microphone,&Protocol::ReadPcm,&Protocol::Cancel,
 &Protocol::EndReply,&Protocol::EndConfirmed,&Protocol::MediaEnded,&Protocol::TransportClosed,
 &Protocol::Poll,&Protocol::ActiveTurns,&Protocol::FlushTicket,&Protocol::AckPlaybackFlush,
 &Protocol::ReturnedTarget};
}
// Deliberately dormant link root. No constructor, task, codec, socket or token.
extern "C" const voice_prep::Api* stackchan_muse_voice_protocol_api(){return &api;}
