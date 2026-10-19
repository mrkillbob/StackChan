import CoreAudio
import Foundation

/// Public aggregate aliases for the existing loopback buses; no driver, IO or default-route change.
enum StackChanAudioDevices {
    static let definitions: [(name: String, uid: String, bus: String)] = [
        ("StackChan Microphone", "local.mrkillbob.StackChan.microphone.v1", AudioDeviceRoute.voiceInputUID),
        ("StackChan Speaker", "local.mrkillbob.StackChan.speaker.v1", AudioDeviceRoute.voiceOutputUID)
    ]
    enum Failure: Error { case coreAudio(OSStatus), wrongTopology, missingBus }
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
        return (value.takeRetainedValue() as? [String]) == [bus] && channels(id, kAudioObjectPropertyScopeInput) == 2 && channels(id, kAudioObjectPropertyScopeOutput) == 2
    }
    static func ensure() throws -> [String] {
        guard AudioRouteCatalog.requiredEndpointsAvailable() else { throw Failure.missingBus }
        var created: [AudioObjectID] = []
        do {
            for d in definitions {
                if let existing = find(d.uid) {
                    guard string(existing, kAudioObjectPropertyName) == d.name, topology(existing, bus: d.bus) else { throw Failure.wrongTopology }
                    continue
                }
                let description: [String: Any] = [kAudioAggregateDeviceNameKey: d.name, kAudioAggregateDeviceUIDKey: d.uid, kAudioAggregateDeviceIsPrivateKey: false, kAudioAggregateDeviceIsStackedKey: false, kAudioAggregateDeviceMainSubDeviceKey: d.bus, kAudioAggregateDeviceSubDeviceListKey: [[kAudioSubDeviceUIDKey: d.bus]]]
                var id: AudioObjectID = 0
                let status = AudioHardwareCreateAggregateDevice(description as CFDictionary, &id)
                guard status == noErr else { throw Failure.coreAudio(status) }; created.append(id)
                // CoreAudio publishes streams asynchronously; bounded metadata-only wait.
                var valid = false
                for _ in 0..<20 { if topology(id, bus: d.bus) { valid = true; break }; Thread.sleep(forTimeInterval: 0.05) }
                guard valid else { throw Failure.wrongTopology }
            }
            return definitions.map { $0.name }
        } catch {
            for id in created.reversed() { _ = AudioHardwareDestroyAggregateDevice(id) }
            throw error
        }
    }
}
