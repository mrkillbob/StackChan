import SwiftUI
struct ContentView: View {
    @ObservedObject var controller: PairingController
    var body: some View {
        VStack(alignment: .leading, spacing: 22) {
            HStack(spacing: 12) {
                Image(systemName: "desktopcomputer.and.arrow.down").font(.title)
                VStack(alignment: .leading, spacing: 3) {
                    Text("Pair StackChan").font(.title2.weight(.semibold))
                    Text(controller.audioApproved ? "USB + Wi-Fi · Confirm to connect microphone and speaker" : "USB + Wi-Fi · Temporary pairing check").font(.caption).foregroundStyle(.secondary)
                }
            }
            if controller.audioApproved {
                Text("Local check only: microphone frames are counted and discarded. No recording or cloud service. Confirming the matching number on both screens connects and permits microphone and speaker for this temporary session.")
                    .font(.caption).foregroundStyle(.secondary).fixedSize(horizontal: false, vertical: true)
            }
            VStack(alignment: .leading, spacing: 12) {
                Text(controller.instruction).font(.body).fixedSize(horizontal: false, vertical: true)
                if let code = controller.code {
                    Text(code).font(.system(size: 48, weight: .semibold, design: .monospaced))
                        .frame(maxWidth: .infinity).padding(.vertical, 10).accessibilityLabel("Comparison number " + code.map(String.init).joined(separator: " "))
                } else if [.opening, .staging].contains(controller.phase) {
                    ProgressView().controlSize(.small)
                }
                TimelineView(.periodic(from: .now, by: 1)) { _ in
                    if let remaining = controller.secondsRemaining() {
                        Text("Session ends in \(remaining) seconds").font(.caption).foregroundStyle(.secondary)
                    }
                }
            }.frame(maxWidth: .infinity, minHeight: 145, alignment: .topLeading)
            HStack {
                Button("Cancel", action: controller.cancel).disabled(!controller.canCancel).keyboardShortcut(.cancelAction)
                Spacer()
                Button(controller.primaryTitle, action: controller.primary).buttonStyle(.borderedProminent).disabled(!controller.canPrimary)
            }
            Text(controller.audioApproved ? "Confirm on both screens to connect and allow one temporary local microphone and speaker session. Use Mute on StackChan to stop. Nothing is saved as permanent trust." : "Pairing keeps StackChan muted. Audio needs separate approval. Nothing is saved as permanent trust.")
                .font(.caption).foregroundStyle(.secondary).fixedSize(horizontal: false, vertical: true)
        }.padding(26).frame(width: 470)
    }
}
