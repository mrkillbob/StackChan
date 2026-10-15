import CoreAudio
import Foundation

/// Inspect public endpoint directions without changing the internal transport buses.
enum StackChanAudioDevices {
    static let definitions: [(name: String, uid: String, bus: String, direction: AudioEndpointDirection)] = [
        ("StackChan Microphone", "local.mrkillbob.StackChan.microphone.v1", AudioDeviceRoute.voiceInputUID, .microphone),
        ("StackChan Speaker", "local.mrkillbob.StackChan.speaker.v1", AudioDeviceRoute.voiceOutputUID, .speaker)
    ]
    enum Failure: Error { case coreAudio(OSStatus), wrongTopology, missingBus, directionalDeviceRequired }
    static func string(_ device: AudioObjectID, _ selector: AudioObjectPropertySelector) -> String? {
        var a = AudioObjectPropertyAddress(mSelector: selector, mScope: kAudioObjectPropertyScopeGlobal, mElement: kAudioObjectPropertyElementMain)
        var v: Unmanaged<CFString>?; var n = UInt32(MemoryLayout.size(ofValue: v))
        guard AudioObjectGetPropertyData(device, &a, 0, nil, &n, &v) == noErr else { return nil }
        return v?.takeRetainedValue() as String?
    }
    static func devices() -> [AudioObjectID] {
        var a = AudioObjectPropertyAddress(mSelector: kAudioHardwarePropertyDevices, mScope: kAudioObjectPropertyScopeGlobal, mElement: kAudioObjectPropertyElementMain)
        var n: UInt32 = 0
        guard AudioObjectGetPropertyDataSize(AudioObjectID(kAudioObjectSystemObject), &a, 0, nil, &n) == noErr, n > 0 else { return [] }
        var ids = [AudioObjectID](repeating: 0, count: Int(n) / MemoryLayout<AudioObjectID>.size)
        guard AudioObjectGetPropertyData(AudioObjectID(kAudioObjectSystemObject), &a, 0, nil, &n, &ids) == noErr else { return [] }
        return ids
    }
    static func find(_ uid: String) -> AudioObjectID? { devices().first { string($0, kAudioDevicePropertyDeviceUID) == uid } }
    static func channels(_ id: AudioObjectID, _ scope: AudioObjectPropertyScope) -> Int {
        var a = AudioObjectPropertyAddress(mSelector: kAudioDevicePropertyStreamConfiguration, mScope: scope, mElement: kAudioObjectPropertyElementMain)
        var n: UInt32 = 0
        guard AudioObjectGetPropertyDataSize(id, &a, 0, nil, &n) == noErr, n >= MemoryLayout<AudioBufferList>.size else { return 0 }
        let p = UnsafeMutableRawPointer.allocate(byteCount: Int(n), alignment: MemoryLayout<AudioBufferList>.alignment); defer { p.deallocate() }
        guard AudioObjectGetPropertyData(id, &a, 0, nil, &n, p) == noErr else { return 0 }
        return UnsafeMutableAudioBufferListPointer(p.assumingMemoryBound(to: AudioBufferList.self)).reduce(0) { $0 + Int($1.mNumberChannels) }
    }
    static func topology(_ id: AudioObjectID, bus: String) -> Bool {
        var a = AudioObjectPropertyAddress(mSelector: kAudioAggregateDevicePropertyFullSubDeviceList, mScope: kAudioObjectPropertyScopeGlobal, mElement: kAudioObjectPropertyElementMain)
        var value: Unmanaged<CFArray>?; var n = UInt32(MemoryLayout.size(ofValue: value))
        guard AudioObjectGetPropertyData(id, &a, 0, nil, &n, &value) == noErr, let value else { return false }
        return (value.takeRetainedValue() as? [String]) == [bus]
    }
    static func streams(_ id: AudioObjectID, _ scope: AudioObjectPropertyScope) -> Int {
        var address = AudioObjectPropertyAddress(mSelector: kAudioDevicePropertyStreams, mScope: scope, mElement: kAudioObjectPropertyElementMain)
        var bytes: UInt32 = 0
        guard AudioObjectGetPropertyDataSize(id, &address, 0, nil, &bytes) == noErr else { return -1 }
        return Int(bytes) / MemoryLayout<AudioStreamID>.size
    }
    static func matchesDirection(_ id: AudioObjectID, _ direction: AudioEndpointDirection) -> Bool {
        direction.accepts(inputChannels: channels(id, kAudioObjectPropertyScopeInput),
                          outputChannels: channels(id, kAudioObjectPropertyScopeOutput),
                          inputStreams: streams(id, kAudioObjectPropertyScopeInput),
                          outputStreams: streams(id, kAudioObjectPropertyScopeOutput))
    }
    static func ensure() throws -> [String] {
        guard AudioRouteCatalog.requiredEndpointsAvailable() else { throw Failure.missingBus }
        for definition in definitions {
            guard let id = find(definition.uid) else { throw Failure.directionalDeviceRequired }
            guard string(id, kAudioObjectPropertyName) == definition.name, topology(id, bus: definition.bus) else { throw Failure.wrongTopology }
            guard matchesDirection(id, definition.direction) else { throw Failure.directionalDeviceRequired }
        }
        // Aggregate creation cannot suppress the backing duplex bus's opposite scope.
        // Never recreate the known-wrong public aliases or claim they are directional.
        return definitions.map { $0.name }
    }
}
