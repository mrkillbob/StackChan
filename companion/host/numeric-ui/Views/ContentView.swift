import SwiftUI
struct ContentView: View {
    @ObservedObject var controller: PairingController
    var body: some View {
        VStack(alignment: .leading, spacing: 22) {
            HStack(spacing: 12) {
                Image(systemName: "desktopcomputer.and.arrow.down").font(.title)
                VStack(alignment: .leading, spacing: 3) {
                    Text("Pair StackChan").font(.title2.weight(.semibold))
                    Text("USB + Wi-Fi · Temporary microphone and speaker access").font(.caption).foregroundStyle(.secondary)
                }
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
            Text("Keep USB connected. Use Mute on the robot to stop audio. Nothing is saved as permanent trust.")
                .font(.caption).foregroundStyle(.secondary).fixedSize(horizontal: false, vertical: true)
        }.padding(26).frame(width: 470)
    }
}
