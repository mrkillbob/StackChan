import Foundation

struct AudioDeviceRoute {
    static let voiceInputUID = "Pro Tools Audio Bridge 2-A_UID"
    static let voiceOutputUID = "Pro Tools Audio Bridge 2-B_UID"

    let endpointsAvailable: Bool

    var canUseStackChan: Bool { endpointsAvailable }

    func workerPayload() -> [String: Any]? {
        guard canUseStackChan else { return nil }
        return [
            "command": "connect",
            "audio_route": [
                "input_uid": Self.voiceInputUID,
                "output_uid": Self.voiceOutputUID
            ]
        ]
    }
}
