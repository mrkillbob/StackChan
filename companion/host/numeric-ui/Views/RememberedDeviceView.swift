import SwiftUI
struct RememberedDeviceView: View {
    @ObservedObject var controller: PairingController
    @State private var deviceFeedback = ""
    var body: some View {
        GroupBox("Sync · Saved Mac pairing") {
            VStack(alignment: .leading, spacing: 8) {
                Text("Saved wireless trust is reused. To use StackChan as normal audio devices, select StackChan Microphone as your input and StackChan Speaker as your output in Sound or your Voice app, then call Mira normally. Sync prepares the listener without requiring a call to be open.").font(.caption).fixedSize(horizontal: false, vertical: true)
                HStack(spacing: 8) {
                    Image(systemName: controller.audioRouteEndpointsAvailable ? "checkmark.circle.fill" : "exclamationmark.circle")
                        .foregroundStyle(controller.audioRouteEndpointsAvailable ? .green : .secondary)
                    Text(controller.audioRouteEndpointsAvailable ? "Both CoreAudio endpoints are available" : "CoreAudio Bridge 2-A/2-B not found")
                        .font(.caption)
                }
                Button("Make StackChan selectable") {
                    do {
                        _ = try StackChanAudioDevices.ensure()
                        deviceFeedback = "Select StackChan Microphone for input and StackChan Speaker for output. Sync and physical Listen are still required."
                    } catch {
                        deviceFeedback = "Could not prepare StackChan devices: \(error). Existing Bridge 2-A input and 2-B output remain available."
                    }
                    controller.refreshAudioRouteAvailability()
                }.disabled(controller.activity != nil || controller.cleanupPending)
                if !deviceFeedback.isEmpty {
                    Text(deviceFeedback).font(.caption).fixedSize(horizontal: false, vertical: true)
                }
                if controller.persistentMode, let code = controller.code {
                    Text(code).font(.system(size: 36, weight: .semibold, design: .monospaced))
                        .accessibilityLabel("Comparison number " + code.map(String.init).joined(separator: " "))
                }
                if controller.persistentMode, let recovery = controller.recovery {
                    Text(recovery.guidance).font(.caption).fixedSize(horizontal: false, vertical: true)
                }
                HStack {
                    Button("Enroll", action: controller.enrollRemembered).disabled(!controller.canEnrollRemembered)
                    Button("Confirm match", action: controller.confirmRemembered).disabled(!controller.persistentMode || controller.phase != .comparing || controller.confirmationSent)
                    Button("Sync", action: controller.useStackChan).disabled(!controller.canUseStackChan)
                    Button("Forget on Mac", action: controller.forgetRemembered).disabled(!controller.canForgetRemembered)
                }
                if controller.persistentMode && controller.activity != nil {
                    Text("Listener stays ready for reconnection. Disconnect closes the listener.").font(.caption).fixedSize(horizontal: false, vertical: true)
                }
                if controller.persistentMode {
                    Button(controller.activity == nil ? "Cancel" : "Disconnect", action: controller.cancel)
                        .disabled(!controller.canCancel)
                }
                Text(controller.rememberedStatus).font(.caption).foregroundStyle(.secondary).fixedSize(horizontal: false, vertical: true)
                if controller.phase == .paired && controller.activity == nil { Text("No microphone capture starts during enrollment. Sync is a separate action; physical Listen remains required.").font(.caption) }
            }.padding(.top, 8)
        }.onAppear { controller.refreshAudioRouteAvailability() }
    }
}
