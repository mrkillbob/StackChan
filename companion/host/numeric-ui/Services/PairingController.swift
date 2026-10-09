import Foundation
import Combine
import Darwin

final class PairingController: ObservableObject {
    @Published private(set) var phase: PairingPhase = .idle
    @Published private(set) var code: String?
    @Published private(set) var remaining: Int?
    @Published private(set) var confirmationSent = false
    @Published private(set) var cleanupPending = false
    let approved: Bool
    let audioApproved: Bool
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
    init(approved: Bool, root: URL, processFactory: (() -> Process)? = nil, audioApproved: Bool = false) { self.approved = approved; self.audioApproved = approved && audioApproved; self.root = root; self.processFactory = processFactory }
    var canConfirm: Bool { approved && phase == .comparing && code != nil && !confirmationSent }
    var primaryTitle: String {
        if phase == .comparing || phase == .waiting_confirmation { return confirmationSent ? "Waiting for StackChan" : "Confirm match" }
        if phase.terminal { return "Retry pairing" }
        return phase == .idle ? "Pair StackChan" : "Pairing…"
    }
    var canPrimary: Bool { approved && !cleanupPending && (phase == .idle || phase.terminal || canConfirm) }
    var canCancel: Bool { approved && phase.busy && !cleanupPending }
    var instruction: String {
        if !approved { return "Prepared preview. Pairing needs approval for one temporary USB/Wi-Fi check." }
        switch phase {
        case .idle: return "Open Companion on StackChan and leave it muted. Click Pair StackChan to begin."
        case .opening: return "Opening USB before pairing. Wait for the robot to settle."
        case .usb_ready, .waiting_robot: return "Keep USB connected. On the robot, tap Pair trusted host."
        case .comparing, .waiting_confirmation: return confirmationSent ? "Confirm the same number on StackChan. Only accept if both numbers match." : "Compare this number with the correct StackChan screen. Confirm on both only if they match."
        case .staging: return "Both confirmations received. Waiting for StackChan’s acknowledgement."
        case .paired: return audioApproved ? "Pairing acknowledged. On StackChan, tap Connect, then Allow for this approved audio check. Tap Mute to stop." : "Pairing acknowledged. Keep StackChan muted. Audio needs separate approval."
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
        confirmationSent = false; code = nil; remaining = nil
        if child == nil { start() }
        if child != nil { phase = .opening; send(retry ? "retry" : "prepare") }
    }
    private func start() {
        let process = processFactory?() ?? Process(), incoming = Pipe(), outgoing = Pipe()
        let activeGeneration = UUID(); generation = activeGeneration; buffer.removeAll(); requiresProcessExit = false
        if processFactory == nil {
        process.executableURL = URL(fileURLWithPath: "/Users/mikedemott/Homelab/.venv/bin/python")
        let worker = root.appendingPathComponent("src/upstream/companion/host/streaming/numeric_pairing_worker.py")
        process.arguments = ["-B", worker.path, "--approved-test", "--port", "/dev/cu.usbmodem31201",
          "--cert", root.appendingPathComponent(".tools/local-companion-tls/server.pem").path,
          "--key", root.appendingPathComponent(".tools/local-companion-tls/server-key.pem").path,
          "--cue", root.appendingPathComponent(".tools/native-call-preparation/transport-speaker-cue.wav").path]
        if audioApproved { process.arguments?.append("--approved-audio") }
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
                if self.phase.busy { self.phase = .failed; self.code = nil }
                self.cleanupPending = false
                let done = self.shutdownDone; self.shutdownDone = nil; done?(true)
            }
        }
        do { try process.run(); child = process; input = incoming.fileHandleForWriting; output = outgoing.fileHandleForReading }
        catch { outgoing.fileHandleForReading.readabilityHandler = nil; phase = .failed }
    }
    private func receive(_ data: Data) {
        guard !data.isEmpty else { return }
        buffer.append(data)
        guard buffer.count <= 8192 else { failProtocol(); return }
        while let end = buffer.firstIndex(of: 10) {
            let line = buffer.subdata(in: 0..<end); buffer.removeSubrange(0...end)
            guard let status = PairingStatus.decode(line) else { failProtocol(); return }
            if requiresProcessExit || (cleanupPending && !status.phase.terminal) { continue }
            phase = status.phase; code = status.code; remaining = status.remaining
            receivedUptime = ProcessInfo.processInfo.systemUptime
            if phase.terminal { confirmationSent = false; cleanupPending = false }
        }
    }
    private func failProtocol() {
        phase = .failed; code = nil; cleanupPending = true
        shutdown { [weak self] closed in self?.cleanupPending = !closed }
    }
    private func send(_ command: String) {
        guard let input = input,
              let data = try? JSONSerialization.data(withJSONObject: ["command": command]) else { return }
        do { try input.write(contentsOf: data + Data([10])) }
        catch { phase = .failed; code = nil; cleanupPending = true; requiresProcessExit = true
            try? self.input?.close(); self.input = nil
            stopAfterRequest { [weak self] closed in self?.cleanupPending = !closed }
        }
    }
    func secondsRemaining(at uptime: TimeInterval = ProcessInfo.processInfo.systemUptime) -> Int? {
        guard let remaining = remaining else { return nil }
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
