import SwiftUI
struct ContentView: View {
    @ObservedObject var controller: PairingController
    var body: some View {
        VStack(alignment: .leading, spacing: 22) {
            HStack(spacing: 12) {
                Image(systemName: "desktopcomputer.and.arrow.down").font(.title)
                VStack(alignment: .leading, spacing: 3) {
                    Text("StackChan Companion").font(.title2.weight(.semibold))
                    Text(controller.persistentMode ? "Remembered pairing · Manual connection" : (controller.audioApproved ? controller.audioSubtitle : "USB + Wi-Fi · Temporary pairing check")).font(.caption).foregroundStyle(.secondary)
                }
            }
            RememberedDeviceView(controller: controller)
            DisclosureGroup("Temporary pairing diagnostic") {
            if controller.audioApproved && !controller.persistentMode {
                Text(controller.audioDisclosure)
                    .font(.caption).foregroundStyle(.secondary).fixedSize(horizontal: false, vertical: true)
            }
            SessionFlowView(stage: controller.stage, readyNow: controller.readyNow)
            VStack(alignment: .leading, spacing: 12) {
                if !controller.persistentMode && controller.canAcknowledgeCall {
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
            Text(controller.persistentMode ? "Enrollment saves wireless trust. Use StackChan prepares the listener; physical Listen on the robot remains the audio gate." : (controller.audioApproved ? "Match the number on both screens. The robot’s Pair & authorize mic/speaker button gives physical consent. Use its physical stop to end the session. Nothing is saved as permanent trust." : "Pairing keeps StackChan muted. Audio starts only after separate physical Listen consent."))
                .font(.caption).foregroundStyle(.secondary).fixedSize(horizontal: false, vertical: true)
            }
            IntegrationReadinessView()
        }.padding(26).frame(width: 520)
    }
}
