import CoreAudio
import Foundation

/// Inspect public endpoint directions without changing the internal transport buses.
enum StackChanAudioDevices {
    static let definitions: [(name: String, uid: String, direction: AudioEndpointDirection, hidden: Bool)] = [
        ("StackChan Microphone", "local.mrkillbob.StackChan.hal.microphone.v2", .microphone, false),
        ("StackChan Speaker", "local.mrkillbob.StackChan.hal.speaker.v2", .speaker, false),
        ("StackChan Internal Feed", "local.mrkillbob.StackChan.hal.feed.v2", .speaker, true),
        ("StackChan Internal Return", "local.mrkillbob.StackChan.hal.return.v2", .microphone, true)
    ]
    enum Failure: Error { case wrongTopology, directionalDeviceRequired, legacyAliasesStillVisible }
    static func string(_ device: AudioObjectID, _ selector: AudioObjectPropertySelector) -> String? {
        var a = AudioObjectPropertyAddress(mSelector: selector, mScope: kAudioObjectPropertyScopeGlobal, mElement: kAudioObjectPropertyElementMain)
        var v: Unmanaged<CFString>?; var n = UInt32(MemoryLayout.size(ofValue: v))
        guard AudioObjectGetPropertyData(device, &a, 0, nil, &n, &v) == noErr else { return nil }
        return v?.takeRetainedValue() as String?
    }
    static func devices() -> [AudioObjectID]? {
        var a = AudioObjectPropertyAddress(mSelector: kAudioHardwarePropertyDevices, mScope: kAudioObjectPropertyScopeGlobal, mElement: kAudioObjectPropertyElementMain)
        var n: UInt32 = 0
        guard AudioObjectGetPropertyDataSize(AudioObjectID(kAudioObjectSystemObject), &a, 0, nil, &n) == noErr,
              n % UInt32(MemoryLayout<AudioObjectID>.size) == 0 else { return nil }
        if n == 0 { return [] }
        var ids = [AudioObjectID](repeating: 0, count: Int(n) / MemoryLayout<AudioObjectID>.size)
        let expected = n
        guard AudioObjectGetPropertyData(AudioObjectID(kAudioObjectSystemObject), &a, 0, nil, &n, &ids) == noErr, n == expected else { return nil }
        return ids
    }
    static func find(_ uid: String) -> AudioObjectID? {
        // Hidden HAL endpoints are not in the system Devices list.
        var a = AudioObjectPropertyAddress(mSelector: kAudioHardwarePropertyTranslateUIDToDevice, mScope: kAudioObjectPropertyScopeGlobal, mElement: kAudioObjectPropertyElementMain)
        var value = uid as CFString; var id = AudioObjectID(kAudioObjectUnknown)
        var n = UInt32(MemoryLayout.size(ofValue: id))
        let qualifierSize = UInt32(MemoryLayout<CFString>.size)
        let status = withUnsafePointer(to: &value) { qualifier in
            AudioObjectGetPropertyData(AudioObjectID(kAudioObjectSystemObject), &a, qualifierSize, qualifier, &n, &id)
        }
        return status == noErr && n == MemoryLayout<AudioObjectID>.size && id != kAudioObjectUnknown ? id : nil
    }
    static func number(_ id: AudioObjectID, _ selector: AudioObjectPropertySelector, _ scope: AudioObjectPropertyScope = kAudioObjectPropertyScopeGlobal) -> UInt32? {
        var a = AudioObjectPropertyAddress(mSelector: selector, mScope: scope, mElement: kAudioObjectPropertyElementMain)
        var value: UInt32 = 0; var n = UInt32(MemoryLayout.size(ofValue: value))
        guard AudioObjectGetPropertyData(id, &a, 0, nil, &n, &value) == noErr, n == MemoryLayout<UInt32>.size else { return nil }
        return value
    }
    static func channels(_ id: AudioObjectID, _ scope: AudioObjectPropertyScope) -> Int {
        var a = AudioObjectPropertyAddress(mSelector: kAudioDevicePropertyStreamConfiguration, mScope: scope, mElement: kAudioObjectPropertyElementMain)
        var n: UInt32 = 0
        guard AudioObjectGetPropertyDataSize(id, &a, 0, nil, &n) == noErr, n >= MemoryLayout<AudioBufferList>.size else { return 0 }
        let p = UnsafeMutableRawPointer.allocate(byteCount: Int(n), alignment: MemoryLayout<AudioBufferList>.alignment); defer { p.deallocate() }
        guard AudioObjectGetPropertyData(id, &a, 0, nil, &n, p) == noErr else { return 0 }
        return UnsafeMutableAudioBufferListPointer(p.assumingMemoryBound(to: AudioBufferList.self)).reduce(0) { $0 + Int($1.mNumberChannels) }
    }
    static func provenance(_ id: AudioObjectID, hidden: Bool) -> Bool {
        guard number(id, kAudioDevicePropertyTransportType) == kAudioDeviceTransportTypeVirtual,
              number(id, kAudioDevicePropertyIsHidden) == (hidden ? 1 : 0),
              let owner = number(id, kAudioObjectPropertyOwner), owner != kAudioObjectUnknown,
              number(owner, kAudioObjectPropertyClass) == kAudioPlugInClassID,
              string(owner, kAudioPlugInPropertyBundleID) == "local.mrkillbob.StackChan.DirectionalAudio" else { return false }
        return true
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
        for definition in definitions {
            guard let id = find(definition.uid) else { throw Failure.directionalDeviceRequired }
            guard string(id, kAudioDevicePropertyDeviceUID) == definition.uid,
                  string(id, kAudioObjectPropertyName) == definition.name,
                  provenance(id, hidden: definition.hidden) else { throw Failure.wrongTopology }
            guard matchesDirection(id, definition.direction) else { throw Failure.directionalDeviceRequired }
        }
        guard let visible = devices() else { throw Failure.wrongTopology }
        var visibleUIDs: [String] = []
        for id in visible {
            guard let uid = string(id, kAudioDevicePropertyDeviceUID) else { throw Failure.wrongTopology }
            visibleUIDs.append(uid)
        }
        guard DirectionalAudioCatalog.accepts(visibleUIDs) else { throw Failure.legacyAliasesStillVisible }
        // Inspection only: no creation/removal, IO, route, rate or default writes.
        return definitions.filter { !$0.hidden }.map { $0.name }
    }
}
