import SwiftUI
struct RememberedDeviceView: View {
    @ObservedObject var controller: PairingController
    var body: some View {
        DisclosureGroup("Remembered pairing · Preview") {
            VStack(alignment: .leading, spacing: 8) {
                Toggle("Remember This Robot (preference only)", isOn: Binding(
                    get: { controller.rememberRequested }, set: { controller.setRememberRequested($0) }))
                Text(controller.rememberFeedback.text).font(.caption).foregroundStyle(.secondary)
                    .fixedSize(horizontal: false, vertical: true)
                Button("Forget Device", action: controller.forgetDevice).disabled(!controller.rememberRequested)
            }.padding(.top, 8)
        }
    }
}
