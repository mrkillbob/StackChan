#pragma once
#include <atomic>
namespace companion {
enum class AudioOwner { None, Mira, Stock };
inline std::atomic<AudioOwner> audio_owner{AudioOwner::None};
inline bool AcquireAudio(AudioOwner owner) {
 if(owner==AudioOwner::None)return false;
 auto expected=AudioOwner::None;
 return audio_owner.compare_exchange_strong(expected,owner) || expected==owner;
}
inline void ReleaseAudio(AudioOwner owner) {
 auto expected=owner;audio_owner.compare_exchange_strong(expected,AudioOwner::None);
}
inline AudioOwner CurrentAudioOwner(){return audio_owner.load();}
} // namespace companion
