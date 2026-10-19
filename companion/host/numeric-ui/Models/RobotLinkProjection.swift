import Foundation

/// Pure mapping from the pairing worker's reported state to robot-link facts. The link is
/// "up" only while the worker reports a ready audio listener; this is not a call, and the
/// worker's physical-consent activity is the only source of consent.
enum RobotLinkProjection {
    static func linked(phase: PairingPhase, activity: PairingActivity?) -> Bool { phase == .paired && activity != nil }
    static func consented(phase: PairingPhase, activity: PairingActivity?) -> Bool { linked(phase: phase, activity: activity) && activity == .conversation }
    /// Fixed-label attribution of a link that ended. `localStop` is true when this Mac's own
    /// Stop/Cancel control caused it; the worker does not say which side stopped.
    static func lossReason(phase: PairingPhase, recovery: PairingRecovery?, localStop: Bool) -> CloseReason {
        if let recovery = recovery {
            switch recovery {
            case .network, .connection: return .wifiLost
            case .reset: return .robotReset
            default: return .protocolError
            }
        }
        switch phase {
        case .expired: return .sessionLimit
        case .cancelled: return localStop ? .userClosed : .robotClosed
        case .closed: return .robotClosed
        default: return .protocolError
        }
    }
}
