import Foundation
@main struct AudioEndpointDirectionChecks {
 static func main() {
  let microphone = AudioEndpointDirection.microphone, speaker = AudioEndpointDirection.speaker
  // Actual previously exposed aggregate topology: rejected for BOTH public roles.
  assert(!microphone.accepts(inputChannels:2,outputChannels:2,inputStreams:1,outputStreams:1))
  assert(!speaker.accepts(inputChannels:2,outputChannels:2,inputStreams:1,outputStreams:1))
  assert(microphone.accepts(inputChannels:2,outputChannels:0,inputStreams:1,outputStreams:0))
  assert(speaker.accepts(inputChannels:0,outputChannels:2,inputStreams:0,outputStreams:1))
  assert(!microphone.accepts(inputChannels:0,outputChannels:2,inputStreams:0,outputStreams:1))
  assert(!speaker.accepts(inputChannels:2,outputChannels:0,inputStreams:1,outputStreams:0))
  // Channel counts alone cannot hide a remaining opposite-direction stream.
  assert(!microphone.accepts(inputChannels:2,outputChannels:0,inputStreams:1,outputStreams:1))
  assert(!speaker.accepts(inputChannels:0,outputChannels:2,inputStreams:1,outputStreams:1))
  for invalid in [-1,0,1,3] { assert(!microphone.accepts(inputChannels:invalid,outputChannels:0,inputStreams:1,outputStreams:0)) }
  assert(!speaker.accepts(inputChannels:0,outputChannels:2,inputStreams:0,outputStreams:-1))
  let observed=[(2,2,1,1),(2,0,1,0),(0,2,0,1)]
  let inputs=observed.indices.filter { let x=observed[$0];return microphone.accepts(inputChannels:x.0,outputChannels:x.1,inputStreams:x.2,outputStreams:x.3) }
  let outputs=observed.indices.filter { let x=observed[$0];return speaker.accepts(inputChannels:x.0,outputChannels:x.1,inputStreams:x.2,outputStreams:x.3) }
  assert(inputs == [1] && outputs == [2])
  let current = Array(DirectionalAudioCatalog.publicUIDs)
  assert(DirectionalAudioCatalog.accepts(current))
  assert(DirectionalAudioCatalog.accepts(current + ["unrelated.device"]))
  assert(!DirectionalAudioCatalog.accepts(nil)) // enumeration failure is unknown
  assert(!DirectionalAudioCatalog.accepts([]))
  assert(!DirectionalAudioCatalog.accepts([current[0]]))
  for forbidden in DirectionalAudioCatalog.forbiddenVisibleUIDs {
   assert(!DirectionalAudioCatalog.accepts(current + [forbidden]))
  }
  print("PASS public direction/enumeration gates; duplex aliases rejected; no audio IO")
 }
}
