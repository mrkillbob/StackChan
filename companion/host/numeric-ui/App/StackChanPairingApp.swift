import SwiftUI
import AppKit
final class AppDelegate: NSObject, NSApplicationDelegate {
    weak var controller: PairingController?
    func applicationDidFinishLaunching(_ notification: Notification) { NSApp.setActivationPolicy(.regular); NSApp.activate(ignoringOtherApps: true) }
    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool { true }
    func applicationShouldTerminate(_ sender: NSApplication) -> NSApplication.TerminateReply {
        guard let controller = controller else { return .terminateNow }
        controller.shutdown { closed in DispatchQueue.main.async { sender.reply(toApplicationShouldTerminate: closed) } }
        return .terminateLater
    }
}
@main struct StackChanPairingApp: App {
    @NSApplicationDelegateAdaptor(AppDelegate.self) private var delegate
    @StateObject private var controller = PairingController(
        approved: CommandLine.arguments.contains("--approved-test"),
        root: URL(fileURLWithPath: "/Users/mikedemott/Developer/AgentWork/StackChan"))
    var body: some Scene {
        Window("Pair StackChan", id: "pairing") {
            ContentView(controller: controller).onAppear { delegate.controller = controller }
        }.windowResizability(.contentSize)
    }
}
