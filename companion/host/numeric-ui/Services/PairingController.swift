import Foundation
import Combine
import Darwin

final class PairingController: ObservableObject {
    @Published private(set) var phase: PairingPhase = .idle
    @Published private(set) var code: String?
    @Published private(set) var remaining: Int?
    @Published private(set) var confirmationSent = false
    @Published private(set) var cleanupPending = false
    @Published private(set) var cleanupIssues: Set<PairingCleanupIssue> = []
    @Published private(set) var activity: PairingActivity?
    @Published private(set) var audioRemaining: Int?
    @Published private(set) var recovery: PairingRecovery?
    @Published private(set) var originalCallConfirmed = false
    @Published private(set) var restorationConfirmed = false
    @Published private(set) var rememberRequested = false
    @Published private(set) var rememberFeedback: RememberedDeviceFeedback = .unavailable
    private var rememberedPairing = RememberedPairing()
    private let restorationVerifier: SessionRestorationVerifying?
    private(set) var attemptID = UUID()
    let approved: Bool
    let audioApproved: Bool
    let audioMode: CompanionAudioMode
    let oneAttempt: Bool
    private let root: URL
    private var child: Process?
    private var input: FileHandle?
    private var output: FileHandle?
    private var buffer = Data()
    private var shutdownDone: ((Bool) -> Void)?
    private var generation = UUID()
    private var requiresProcessExit = false
    private var receivedUptime = ProcessInfo.processInfo.systemUptime
    private let processFactory: (() -> Process)?
    init(approved: Bool, root: URL, processFactory: (() -> Process)? = nil, audioApproved: Bool = false, oneAttempt: Bool = true, audioMode: CompanionAudioMode = .transportTest, restorationVerifier: SessionRestorationVerifying? = nil) { self.restorationVerifier = restorationVerifier; self.audioMode = audioMode; self.oneAttempt = oneAttempt; self.approved = approved; self.audioApproved = approved && audioApproved; self.root = root; self.processFactory = processFactory }
    var audioDisclosure: String {
        if audioMode == .nativeConversation {
            return "Microphone audio goes to your original dot voice call. That call's audio plays on StackChan. This app does not record audio. Match the number on both screens. The robot’s Pair & authorize mic/speaker confirmation gives physical consent for this temporary session."
        }
        return "Transport test: microphone frames are counted and discarded locally. The prepared test cue plays on StackChan. No audio recording or forwarding to a voice call. Confirm the matching number on both screens to permit this temporary test."
    }
    var audioSubtitle: String {
        audioMode == .nativeConversation ? "Original dot call · Microphone and speaker" : "Transport test · Local cue and microphone discard"
    }
    var workerLaunchArguments: [String] {
        let worker = root.appendingPathComponent("src/upstream/companion/host/streaming/numeric_pairing_worker.py")
        var args = ["-B", worker.path, "--approved-test", "--port", "/dev/cu.usbmodem31201",
            "--cert", root.appendingPathComponent(".tools/local-companion-tls/server.pem").path,
            "--key", root.appendingPathComponent(".tools/local-companion-tls/server-key.pem").path,
            "--cue", root.appendingPathComponent(".tools/native-call-preparation/transport-speaker-cue.wav").path]
        if audioApproved { args += ["--approved-audio", "--audio-mode", audioMode.rawValue] }
        return args
    }
    var requiresOriginalCall: Bool { audioApproved && audioMode == .nativeConversation && !originalCallConfirmed }
    var stage: CompanionStage {
        if !approved { return .preparing }
        if cleanupPending || phase == .restoring || !cleanupIssues.isEmpty { return .restoring }
        if phase.terminal {
            return audioApproved && audioMode == .nativeConversation && !restorationConfirmed ? .restoring : .closed
        }
        if requiresOriginalCall && phase == .idle { return .callRequired }
        if phase == .idle || phase == .opening { return .preparing }
        if phase == .paired && audioApproved { return activity == .conversation ? .conversation : .physicalConsent }
        return .pairing
    }
    var readyNow: Bool { approved && phase == .idle && !requiresOriginalCall && !cleanupPending && cleanupIssues.isEmpty }
    var canAcknowledgeCall: Bool { approved && requiresOriginalCall && phase == .idle && !cleanupPending }
    func acknowledgeOriginalCall() {
        guard canAcknowledgeCall else { return }
        originalCallConfirmed = true
    }
    func refreshRestoration() {
        guard phase.terminal, child == nil, cleanupIssues.isEmpty, !cleanupPending,
              restorationVerifier?.receipt(for: attemptID)?.verifies(attemptID) == true else { return }
        restorationConfirmed = true
    }
    func setRememberRequested(_ enabled: Bool) {
        rememberedPairing.requestRemember(enabled)
        rememberRequested = rememberedPairing.requested; rememberFeedback = rememberedPairing.feedback
    }
    func forgetDevice() {
        rememberedPairing.forget()
        rememberRequested = rememberedPairing.requested; rememberFeedback = rememberedPairing.feedback
    }
    var canConfirm: Bool { approved && phase == .comparing && code != nil && !confirmationSent }
    var primaryTitle: String {
        if !cleanupIssues.isEmpty { return "Cleanup needed" }
        if requiresOriginalCall && phase == .idle { return "Call required" }
        if stage == .conversation { return "Conversation active" }
        if stage == .restoring { return "Verifying cleanup" }
        if phase == .comparing || phase == .waiting_confirmation { return confirmationSent ? "Waiting for StackChan" : (audioApproved ? "Confirm & connect" : "Confirm match") }
        if phase.terminal { return oneAttempt ? "New approval needed" : "Retry pairing" }
        return phase == .idle ? "Pair StackChan" : "Pairing…"
    }
    var canPrimary: Bool { approved && !requiresOriginalCall && !requiresProcessExit && !cleanupPending && cleanupIssues.isEmpty && !(phase.terminal && stage == .restoring) && (phase == .idle || (phase.terminal && !oneAttempt) || canConfirm) }
    var canCancel: Bool { approved && phase.busy && phase != .restoring && !cleanupPending }
    var instruction: String {
        if !approved { return "Prepared preview. Pairing needs approval for one temporary USB/Wi-Fi check." }
        if !cleanupIssues.isEmpty {
            var checks = [String]()
            if cleanupIssues.contains(.audio) { checks.append("confirm the original microphone and speaker routes are restored, keeping StackChan muted") }
            if cleanupIssues.contains(.grant) { checks.append("verify the temporary pairing grant was revoked") }
            if cleanupIssues.contains(.usb) { checks.append("verify the USB pairing session is closed") }
            return "Cleanup is not confirmed. Do not try again until you " + checks.joined(separator: ", and ") + "."
        }
        if stage == .restoring {
            return "The local session is stopping or closed. Keep StackChan muted. The operator must independently verify original microphone/speaker routes and owned process, USB and listener closure. This app cannot yet verify that supervisor receipt."
        }
        if requiresOriginalCall && phase == .idle {
            return "After the operator completes the approved route preparation, start a fresh voice call in this same original dot conversation. Confirm it is open below. Automatic call launch is unavailable."
        }
        if let recovery = recovery { return recovery.guidance }
        if phase.terminal && oneAttempt { return "Session ended. Keep StackChan muted. A fresh approved check is needed before another attempt." }
        switch phase {
        case .idle: return "Ready now. Open Companion on StackChan and leave it muted. Click Pair StackChan to begin."
        case .opening: return "Opening USB before pairing. Wait for the robot to settle."
        case .usb_ready, .waiting_robot: return "Keep USB connected. On the robot, tap Pair."
        case .comparing, .waiting_confirmation:
            if audioApproved { return confirmationSent ? "Confirm the same number on StackChan only if both numbers match. Its Pair & authorize mic/speaker button gives physical consent for the temporary microphone and speaker session." : "Compare this number with the correct StackChan screen. Confirm on both only if they match. The robot’s Pair & authorize mic/speaker button gives physical consent for the temporary microphone and speaker session." }
            return confirmationSent ? "Confirm the same number on StackChan. Only accept if both numbers match." : "Compare this number with the correct StackChan screen. Confirm on both only if they match."
        case .staging: return "Both confirmations received. Waiting for StackChan’s acknowledgement."
        case .restoring: return "Closing temporary access. Keep StackChan muted while cleanup is verified."
        case .paired:
            if audioApproved && audioMode == .nativeConversation && activity == .conversation {
                return "Physical mic consent was accepted. Your original call audio is being bridged for this bounded trial. Mira identity, conversation continuity and physical playback still need human confirmation. Use Cancel or the physical stop to end."
            }
            return audioApproved ? "Pairing confirmed. Waiting for the robot’s physical microphone consent acknowledgement from its matching-number confirmation. Use Cancel on this Mac or Back on StackChan to stop." : "Pairing acknowledged. Keep StackChan muted. Audio needs separate approval."
        case .cancelled: return "Cancelled. Any temporary grant is revoked. Retry starts a fresh attempt."
        case .expired: return "The session expired. Retry starts a fresh attempt; no previous grant is reused."
        case .failed: return "Pairing stopped. Check the robot is muted and connected before a fresh retry."
        case .closed: return "Session closed. Temporary access ended."
        }
    }
    func primary() {
        guard canPrimary else { return }
        if canConfirm { confirmationSent = true; send("confirm"); return }
        let retry = phase.terminal && child != nil
        confirmationSent = false; code = nil; remaining = nil; audioRemaining = nil; activity = nil; recovery = nil; restorationConfirmed = false; attemptID = UUID()
        if child == nil { start() }
        if child != nil { phase = .opening; send(retry ? "retry" : "prepare") }
    }
    private func start() {
        let process = processFactory?() ?? Process(), incoming = Pipe(), outgoing = Pipe()
        // A worker may close stdin before terminationHandler reaches the main queue.
        // Suppress SIGPIPE only on our command descriptor so EPIPE reaches cleanup.
        guard fcntl(incoming.fileHandleForWriting.fileDescriptor, F_SETNOSIGPIPE, 1) == 0 else {
            try? incoming.fileHandleForReading.close(); try? incoming.fileHandleForWriting.close()
            try? outgoing.fileHandleForReading.close(); try? outgoing.fileHandleForWriting.close()
            phase = .failed; recovery = .setup; return
        }
        let activeGeneration = UUID(); generation = activeGeneration; buffer.removeAll(); requiresProcessExit = false
        if processFactory == nil {
        process.executableURL = URL(fileURLWithPath: "/Users/mikedemott/Homelab/.venv/bin/python")
        process.arguments = workerLaunchArguments
        process.currentDirectoryURL = root
        var environment = ProcessInfo.processInfo.environment
        environment["PYTHONDONTWRITEBYTECODE"] = "1"
        environment["PYTHONPATH"] = ["/Users/mikedemott/Homelab/src", root.appendingPathComponent("bridge/host-adapter").path,
          root.appendingPathComponent("bridge/local-endpoint").path, root.appendingPathComponent(".tools/bridge-python").path].joined(separator: ":")
        process.environment = environment
        }
        process.standardInput = incoming; process.standardOutput = outgoing
        // Raw exceptions/IO are never reflected into the UI or a log file.
        process.standardError = FileHandle.nullDevice
        outgoing.fileHandleForReading.readabilityHandler = { [weak self] handle in
            let data = handle.availableData
            DispatchQueue.main.async {
                guard self?.generation == activeGeneration else { return }
                self?.receive(data)
            }
        }
        process.terminationHandler = { [weak self] process in
            DispatchQueue.main.async {
                guard let self = self, self.generation == activeGeneration, self.child === process else { return }
                self.generation = UUID()
                self.buffer.removeAll()
                self.output?.readabilityHandler = nil; self.output = nil; self.input = nil; self.child = nil
                if self.phase.busy { self.phase = .failed; self.code = nil; self.recovery = self.recovery ?? .workerExit }
                self.cleanupPending = false; self.requiresProcessExit = false
                self.refreshRestoration()
                let done = self.shutdownDone; self.shutdownDone = nil; done?(true)
            }
        }
        do { try process.run(); child = process; input = incoming.fileHandleForWriting; output = outgoing.fileHandleForReading }
        catch { outgoing.fileHandleForReading.readabilityHandler = nil; phase = .failed; recovery = .setup }
    }
    private func receive(_ data: Data) {
        guard !data.isEmpty else { return }
        buffer.append(data)
        guard buffer.count <= 8192 else { failProtocol(); return }
        while let end = buffer.firstIndex(of: 10) {
            let line = buffer.subdata(in: 0..<end); buffer.removeSubrange(0...end)
            guard let status = PairingStatus.decode(line) else { failProtocol(); return }
            if (requiresProcessExit && !status.phase.terminal && status.phase != .restoring) || (cleanupPending && !status.phase.terminal && status.phase != .restoring) { continue }
            phase = status.phase; code = status.code; remaining = status.remaining
            activity = status.activity; audioRemaining = status.audioRemaining
            if let cause = status.recovery, recovery == nil { recovery = cause }
            if phase == .restoring { cleanupPending = true }
            if !status.cleanupIssues.isEmpty { cleanupIssues = status.cleanupIssues }
            receivedUptime = ProcessInfo.processInfo.systemUptime
            if phase.terminal { confirmationSent = false; cleanupPending = !cleanupIssues.isEmpty }
        }
    }
    private func failProtocol() {
        phase = .failed; code = nil; recovery = .protocolFailure; cleanupPending = true
        shutdown { [weak self] closed in self?.cleanupPending = !closed }
    }
    private func send(_ command: String) {
        guard let input = input,
              let data = try? JSONSerialization.data(withJSONObject: ["command": command]) else { return }
        do { try input.write(contentsOf: data + Data([10])) }
        catch { phase = .failed; code = nil; recovery = recovery ?? .connection; cleanupPending = true; requiresProcessExit = true
            try? self.input?.close(); self.input = nil
            stopAfterRequest { [weak self] closed in self?.cleanupPending = !closed }
        }
    }
    func secondsRemaining(at uptime: TimeInterval = ProcessInfo.processInfo.systemUptime) -> Int? {
        guard let remaining = activity == .conversation ? audioRemaining : remaining else { return nil }
        return max(0, remaining - Int(max(0, uptime - receivedUptime)))
    }
    func cancel() { guard canCancel else { return }; cleanupPending = true; code = nil; send("cancel") }
    func shutdown(_ done: @escaping (Bool) -> Void) {
        guard child != nil else { done(true); return }
        cleanupPending = true; requiresProcessExit = true; send("cancel"); try? input?.close(); input = nil
        stopAfterRequest(done)
    }
    private func stopAfterRequest(_ done: @escaping (Bool) -> Void) {
        guard let process = child else { done(true); return }
        shutdownDone = done
        let activeGeneration = generation
        DispatchQueue.main.asyncAfter(deadline: .now() + 4) { [weak self, weak process] in
            guard let self = self, self.generation == activeGeneration,
                  let process = process, self.child === process, process.isRunning else { return }
            process.terminate()
        }
        DispatchQueue.main.asyncAfter(deadline: .now() + 6) { [weak self, weak process] in
            guard let self = self, self.generation == activeGeneration,
                  let process = process, self.child === process, process.isRunning else { return }
            Darwin.kill(process.processIdentifier, SIGKILL)
        }
        DispatchQueue.main.asyncAfter(deadline: .now() + 8) { [weak self, weak process] in
            guard let self = self, self.generation == activeGeneration else { return }
            // Refuse app termination/retry until this exact child is known closed.
            let closed = process == nil || process?.isRunning == false
            if closed { self.cleanupPending = false }
            let completion = self.shutdownDone; self.shutdownDone = nil; completion?(closed)
        }
    }
}
