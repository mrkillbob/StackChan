import SwiftUI
/// Root: Mira home is always the launch surface. Pairing opens only from its button and
/// returns home when this process's pairing reaches .paired. Diagnostics live only in the
/// pairing flow.
struct ContentView: View {
    @ObservedObject var controller: PairingController
    @ObservedObject var lifecycle: CompanionLifecycle
    @State private var showPairing = false
    var body: some View {
        Group {
            if !showPairing {
                MiraLandingView(lifecycle: lifecycle, controller: controller, repair: { showPairing = true })
            } else {
                VStack(spacing: 0) {
                    PairingContentView(controller: controller)
                    // Keep a live worker/session visible until it completes or is cancelled.
                    // Hiding the pairing view must not leave pairing or audio running behind Home.
                    if !controller.phase.busy {
                        Button("Back to Mira") { showPairing = false }
                            .buttonStyle(.link).font(.caption).padding(.bottom, 14)
                    }
                }
            }
        }.onChange(of: controller.phase) { _, newPhase in
            if newPhase == .paired { showPairing = false }
        }
    }
}
struct PairingContentView: View {
    @ObservedObject var controller: PairingController
    var body: some View {
        VStack(alignment: .leading, spacing: 22) {
            HStack(spacing: 12) {
                Image(systemName: "desktopcomputer.and.arrow.down").font(.title)
                VStack(alignment: .leading, spacing: 3) {
                    Text("StackChan Companion").font(.title2.weight(.semibold))
                    Text(controller.audioApproved ? controller.audioSubtitle : "USB + Wi-Fi · Temporary pairing check").font(.caption).foregroundStyle(.secondary)
                }
            }
            if controller.audioApproved {
                Text(controller.audioDisclosure)
                    .font(.caption).foregroundStyle(.secondary).fixedSize(horizontal: false, vertical: true)
            }
            SessionFlowView(stage: controller.stage, readyNow: controller.readyNow)
            VStack(alignment: .leading, spacing: 12) {
                if controller.canAcknowledgeCall {
                    Button("My original call is open", action: controller.acknowledgeOriginalCall)
                }
                Text(controller.instruction).font(.body).fixedSize(horizontal: false, vertical: true)
                if let code = controller.code {
                    Text(code).font(.system(size: 48, weight: .semibold, design: .monospaced))
                        .frame(maxWidth: .infinity).padding(.vertical, 10).accessibilityLabel("Comparison number " + code.map(String.init).joined(separator: " "))
                } else if [.opening, .staging, .restoring].contains(controller.phase) {
                    ProgressView().controlSize(.small)
                }
                TimelineView(.periodic(from: .now, by: 1)) { _ in
                    if let remaining = controller.secondsRemaining() {
                        Text(controller.activity == .conversation ? "Conversation limit: \(remaining) seconds left" : "Pairing window: \(remaining) seconds left").font(.caption).foregroundStyle(.secondary)
                    }
                }
            }.frame(maxWidth: .infinity, minHeight: 145, alignment: .topLeading)
            if let recovery = controller.recovery {
                Text(recovery.guidance).font(.caption).foregroundStyle(.secondary)
                    .fixedSize(horizontal: false, vertical: true)
            }
            HStack {
                Button("Cancel", action: controller.cancel).disabled(!controller.canCancel).keyboardShortcut(.cancelAction)
                Spacer()
                Button(controller.primaryTitle, action: controller.primary).buttonStyle(.borderedProminent).disabled(!controller.canPrimary)
            }
            Text(controller.audioApproved ? "Match the number on both screens. The robot’s Pair & authorize mic/speaker button gives physical consent. Use its physical stop to end the session. Nothing is saved as permanent trust." : "Pairing keeps StackChan muted. Audio needs separate approval. Nothing is saved as permanent trust.")
                .font(.caption).foregroundStyle(.secondary).fixedSize(horizontal: false, vertical: true)
            RememberedDeviceView(controller: controller)
            IntegrationReadinessView()
        }.padding(26).frame(width: 520)
    }
}
