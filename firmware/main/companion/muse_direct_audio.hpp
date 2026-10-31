// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "audio_owner.hpp"
#include <functional>
namespace companion::muse {
// Resource custody only. This class never opens a codec, starts a microphone,
// enables power or submits voice. Both separate live grants must remain true.
class VoiceLease {
    AudioLease lease_;
    std::function<bool()> current_,microphone_,speaker_;
public:
    bool Reserve(uint64_t epoch,std::function<bool()> current,std::function<bool()> microphone,std::function<bool()> speaker){
        if(lease_||!current||!microphone||!speaker||!current()||!microphone()||!speaker())return false;
        if(!AcquireAudio(AudioOwner::Muse,AudioPurpose::MusePushToTalk,epoch,lease_))return false;
        current_=std::move(current);microphone_=std::move(microphone);speaker_=std::move(speaker);return true;
    }
    bool Current(){const bool okay=lease_&&CommonAudioAuthority().Current(lease_)&&current_&&current_()&&microphone_&&microphone_()&&speaker_&&speaker_();if(!okay&&lease_)RevokeAudio(lease_);return okay;}
    bool Stop(bool directions_closed_and_worker_joined){if(!lease_)return true;RevokeAudio(lease_);if(!ReleaseAudio(lease_,directions_closed_and_worker_joined))return false;current_={};microphone_={};speaker_={};return true;}
};
}
