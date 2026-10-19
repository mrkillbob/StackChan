import Foundation

/// The words the Mira home shows for each lifecycle state, kept free of SwiftUI so tests can
/// check that the UI stays truthful. Only a route-verified `.connected` may say "In call".
enum MiraPresentation {
    static func status(_ presence: MiraPresence) -> String {
        switch presence {
        case .connected: return "In call with Mira"
        case .callAsserted: return "Robot connected · call set by you (not verified)"
        case .robotConnected: return "Robot connected · call not attached"
        case .connecting: return "Connecting to StackChan…"
        case .reconnecting(let n, let of): return "Reconnecting (\(n) of \(of)) · muted"
        case .closing: return "Closing…"
        case .closed: return "Closed"
        case .blocked: return "Not connected"
        case .readyToConnect: return "Ready"
        case .cleanupIncomplete: return "Cleanup not confirmed"
        case .trustUnknown: return "Pairing status unavailable"
        case .needsPairing, .repairRequired: return "Pairing needed"
        }
    }

    static func detail(_ presence: MiraPresence, muted: Bool, closeRecord: CloseRecord?, lastFailure: CloseReason?) -> String {
        switch presence {
        case .blocked(let missing): return "Companion is paired, but it cannot join your existing Mira call yet. " + missing
        case .cleanupIncomplete(let steps):
            return "Not confirmed released: " + steps.sorted().joined(separator: ", ") + ". Keep the robot muted and verify before relaunching."
        case .connected: return muted ? "Muted. The microphone is not sent." : "Unmuted."
        case .robotConnected:
            return "StackChan is connected to this Mac and muted. Companion is not attached to your Mira call and cannot start, verify or hang it up. Start or join the call yourself and choose StackChan Microphone and StackChan Speaker there."
        case .callAsserted:
            return "You said your call is running on this Mac. Companion cannot see it. " + (muted ? "Muted. The microphone is not sent." : "Unmuted after robot consent.")
        case .connecting: return "Opening the link to StackChan. Nothing is captured."
        case .reconnecting: return "The link to StackChan dropped. Retrying a few times, always muted, with a fresh session. The robot must give physical consent again."
        case .readyToConnect:
            if let failure = lastFailure { return "Last attempt did not connect because \(failure.label). Nothing is captured until you connect, the robot gives physical consent and you unmute." }
            return "Nothing is captured until you connect, the robot gives physical consent and you unmute."
        case .trustUnknown: return "Companion could not read its saved pairing. It is not treated as paired or unpaired. Quit and reopen, or pair again if this persists."
        case .repairRequired: return "The saved pairing has expired or is incomplete on this Mac. Pair again when you are ready; nothing starts a call."
        case .needsPairing: return "No saved pairing is available on this Mac. Pair deliberately when you are ready; nothing starts a call."
        case .closed: return closeRecord?.message ?? ""
        case .closing: return ""
        }
    }
}
