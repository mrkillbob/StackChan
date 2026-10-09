import Foundation
import CoreFoundation

enum PairingPhase: String {
    case idle, opening, usb_ready, waiting_robot, comparing, waiting_confirmation
    case staging, paired, cancelled, expired, failed, closed
    var terminal: Bool { [.cancelled, .expired, .failed, .closed].contains(self) }
    var busy: Bool { self != .idle && !terminal }
}
struct PairingStatus {
    let phase: PairingPhase
    let code: String?
    let acknowledged: Bool
    let remaining: Int?
    static func decode(_ data: Data) -> PairingStatus? {
        guard data.count <= 1024,
              let object = try? JSONSerialization.jsonObject(with: data) as? [String: Any],
              Set(object.keys).isSubset(of: ["state", "code", "message", "acknowledged", "remaining"]),
              let state = object["state"] as? String, let phase = PairingPhase(rawValue: state) else { return nil }
        let code = object["code"] as? String
        if let code = code {
            guard phase == .comparing || phase == .waiting_confirmation,
                  code.utf8.count == 6, code.utf8.allSatisfy({ $0 >= 48 && $0 <= 57 }) else { return nil }
        }
        if object["code"] != nil && code == nil { return nil }
        var remaining: Int? = nil
        if let value = object["remaining"] {
            guard let number = value as? NSNumber,
                  CFGetTypeID(number) != CFBooleanGetTypeID(),
                  number.doubleValue.rounded() == number.doubleValue,
                  (0...120).contains(number.intValue) else { return nil }
            remaining = number.intValue
        }
        var ack = false
        if let value = object["acknowledged"] {
            guard let number = value as? NSNumber, CFGetTypeID(number) == CFBooleanGetTypeID() else { return nil }
            ack = number.boolValue
        }
        if phase == .paired && !ack { return nil }
        return PairingStatus(phase: phase, code: code, acknowledged: ack, remaining: remaining)
    }
}
