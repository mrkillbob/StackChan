import SwiftUI
import AppKit

/// Daily home. No diagnostics or listener. Pairing is a deliberate button here. The
/// character is the handoff-approved Mira frame (mira-idle-frame.png, SHA-256
/// 93432bf8c86216c9c164d026ab96568cfd966a760a55e12da165af5a35e4ce79), bundled unmodified.
struct MiraLandingView: View {
    @ObservedObject var lifecycle: CompanionLifecycle
    @ObservedObject var controller: PairingController
    private var session: MiraHomeSession {
        MiraHomeSession(phase: controller.phase, cleanupUnconfirmed: controller.cleanupPending || !controller.cleanupIssues.isEmpty || controller.stage == .restoring)
    }
    var repair: () -> Void
    private static let miraImage: NSImage? = Bundle.main.url(forResource: "mira-idle-frame", withExtension: "png").flatMap { NSImage(contentsOf: $0) }
    private var pairTitle: String {
        switch lifecycle.presence {
        case .needsPairing, .repairRequired: return "Pair with Mira…"
        default: return "Pair again…"
        }
    }
    var body: some View {
        VStack(spacing: 16) {
            if let image = Self.miraImage {
                Image(nsImage: image).resizable().scaledToFit().frame(maxHeight: 220).accessibilityLabel("Mira")
            }
            Text("Mira").font(.largeTitle.weight(.semibold))
            HStack(spacing: 8) {
                Circle().fill(lifecycle.presence.showsActiveCall ? Color.green : Color.secondary).frame(width: 10, height: 10)
                Text(status).font(.headline)
            }
            Text(detail).font(.callout).foregroundStyle(.secondary).multilineTextAlignment(.center).fixedSize(horizontal: false, vertical: true)
            if lifecycle.canOpen { Button("Connect to Mira") { lifecycle.open() }.buttonStyle(.borderedProminent) }
            if lifecycle.presence == .connected {
                Toggle("Muted", isOn: Binding(get: { lifecycle.muted }, set: { lifecycle.setMuted($0) }))
                if !lifecycle.physicalConsent { Text("Waiting for the robot’s physical consent.").font(.caption).foregroundStyle(.secondary) }
            }
            if session.pairedThisSession {
                Text("Paired for this session only. Nothing is remembered, and the call and speaker are not verified.").font(.caption).foregroundStyle(.secondary).multilineTextAlignment(.center).fixedSize(horizontal: false, vertical: true)
            }
            if session.showsControls {
                VStack(spacing: 10) {
                    Text(controller.instruction).font(.callout).fixedSize(horizontal: false, vertical: true)
                    TimelineView(.periodic(from: .now, by: 1)) { _ in
                        if let remaining = controller.secondsRemaining() {
                            Text("Session limit: \(remaining) seconds left").font(.caption)
                        }
                    }
                    Button("Stop session", action: controller.cancel).disabled(!controller.canCancel)
                    Text("The robot’s physical stop also ends the session.").font(.caption).foregroundStyle(.secondary)
                }
            }
            Button(pairTitle, action: repair).buttonStyle(.link).font(.caption)
                .disabled(session.showsControls || lifecycle.presence == .connecting || lifecycle.presence == .closing)
        }.padding(26).frame(minWidth: 420, maxWidth: 420, minHeight: 260)
    }
    private var status: String {
        if session.pairedThisSession { return "Paired for this session" }
        if session.showsControls { return "Session cleanup pending" }
        switch lifecycle.presence {
        case .connected: return "In call with Mira"
        case .connecting: return "Connecting…"
        case .closing: return "Closing…"
        case .closed: return "Closed"
        case .blocked: return "Not connected"
        case .readyToConnect: return "Ready"
        case .cleanupIncomplete: return "Cleanup not confirmed"
        case .needsPairing, .repairRequired: return "Pairing needed"
        }
    }
    private var detail: String {
        if session.pairedThisSession { return "The pairing is temporary. Companion cannot verify or join your existing Mira call through a supported route; call continuity and speaker playback remain unverified." }
        switch lifecycle.presence {
        case .blocked(let missing): return "Companion is paired, but it cannot join your existing Mira call yet. " + missing
        case .cleanupIncomplete(let steps): return "Not confirmed released: " + steps.sorted().joined(separator: ", ") + ". Keep the robot muted and verify before relaunching."
        case .connected: return lifecycle.muted ? "Muted. The microphone is not sent." : "Unmuted."
        case .readyToConnect: return "Nothing is captured until you connect, the robot gives physical consent and you unmute."
        case .needsPairing, .repairRequired: return "No remembered pairing is available on this Mac. Pair deliberately when you are ready; nothing starts a call."
        default: return ""
        }
    }
}
