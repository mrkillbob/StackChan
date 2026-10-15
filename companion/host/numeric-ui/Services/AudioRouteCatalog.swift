import CoreAudio
import Foundation

enum AudioRouteCatalog {
    static func requiredEndpointsAvailable() -> Bool {
        let required = Set([AudioDeviceRoute.voiceInputUID, AudioDeviceRoute.voiceOutputUID])
        let systemObject = AudioObjectID(kAudioObjectSystemObject)
        var address = AudioObjectPropertyAddress(
            mSelector: kAudioHardwarePropertyDevices,
            mScope: kAudioObjectPropertyScopeGlobal,
            mElement: kAudioObjectPropertyElementMain
        )
        var size: UInt32 = 0
        guard AudioObjectGetPropertyDataSize(systemObject, &address, 0, nil, &size) == noErr,
              size > 0, size % UInt32(MemoryLayout<AudioObjectID>.size) == 0 else { return false }
        var devices = [AudioObjectID](repeating: 0, count: Int(size) / MemoryLayout<AudioObjectID>.size)
        guard AudioObjectGetPropertyData(systemObject, &address, 0, nil, &size, &devices) == noErr else { return false }
        let found = Set(devices.compactMap(deviceUID))
        return required.isSubset(of: found)
    }

    private static func deviceUID(_ device: AudioObjectID) -> String? {
        var address = AudioObjectPropertyAddress(
            mSelector: kAudioDevicePropertyDeviceUID,
            mScope: kAudioObjectPropertyScopeGlobal,
            mElement: kAudioObjectPropertyElementMain
        )
        var value: Unmanaged<CFString>?
        var size = UInt32(MemoryLayout<Unmanaged<CFString>?>.size)
        guard AudioObjectGetPropertyData(device, &address, 0, nil, &size, &value) == noErr,
              let value else { return nil }
        return value.takeRetainedValue() as String
    }
}
