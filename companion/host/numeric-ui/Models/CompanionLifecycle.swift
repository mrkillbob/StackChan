import Foundation
import Combine

// todo20 shell lifecycle. This file grants no audio authority. It only decides what
// the Companion may truthfully show, enforces close ordering, and keeps late callbacks
// from an earlier open from touching a newer one. Robot physical consent, the OS
// microphone permission and the real audio worker remain owned by existing code.

/// Remembered trust as seen by the launch gate. Production has no durable store
/// (see RememberedPairing.mayAuthorizeSession == false), so it reports `.absent`.
enum LaunchTrust: Equatable { case absent, stale, valid }
protocol LaunchTrustProviding { func current() -> LaunchTrust }
struct UnavailableLaunchTrust: LaunchTrustProviding { func current() -> LaunchTrust { .absent } }

/// Mira home is always the launch surface. Trust only changes what it truthfully says
/// (pairing needed vs ready/blocked); pairing is a deliberate action from home.
enum LaunchDestination: Equatable {
    case miraLanding
    static func decide(_ trust: LaunchTrust) -> LaunchDestination { .miraLanding }
}

/// Home must retain the worker's controls until its session and cleanup have ended.
/// Pairing is temporary session state, never a durable trust or call-connection receipt.
struct MiraHomeSession {
    let phase: PairingPhase
    let cleanupUnconfirmed: Bool
    var pairedThisSession: Bool { phase == .paired }
    var showsControls: Bool { phase.busy || cleanupUnconfirmed }
}

/// What a same-call route can really do. `unsupported` is the production truth: no
/// start-original-Dot-call, hangup or external raw-audio attachment API is available.
enum SameCallCapability: Equatable {
    case unsupported(missing: String)
    case supported(routeID: String, hangup: Bool, returnAudio: Bool)
    var isSupported: Bool { if case .supported = self { return true }; return false }
    var canHangUp: Bool { if case .supported(_, let h, _) = self { return h }; return false }
    var returnsAudio: Bool { if case .supported(_, _, let r) = self { return r }; return false }
}
enum SameCallEvent: Equatable { case connected, failed, ended }

protocol SameCallRoute: AnyObject {
    var capability: SameCallCapability { get }
    /// Starts an attempt. Results must be reported back through `report` with the same epoch.
    func connect(epoch: UInt64, report: @escaping (UInt64, SameCallEvent) -> Void)
    /// Requests hangup of exactly this epoch's call. Only called when `capability.canHangUp`.
    func hangUp(epoch: UInt64, completion: @escaping (Bool) -> Void)
}

/// The only production route: honest about the missing capability.
final class UnsupportedSameCallRoute: SameCallRoute {
    static let missing = "A supported way to start or join the user's existing Mira call and hang it up (and to attach raw two-way audio to it) is not available. WSS/PCM readiness and the local native_session_ref do not constitute a call."
    var capability: SameCallCapability { .unsupported(missing: Self.missing) }
    func connect(epoch: UInt64, report: @escaping (UInt64, SameCallEvent) -> Void) { }
    func hangUp(epoch: UInt64, completion: @escaping (Bool) -> Void) { completion(true) }
}

/// Owned local resources. The worker owns capture, playback and the socket, so the only
/// supported stop is its single atomic shutdown. `done(true)` means the exact worker exit
/// confirmed release; there are no separate mute/capture/playback confirmations.
protocol OwnedAudioResources: AnyObject {
    func shutdownAndConfirmRelease(_ done: @escaping (Bool) -> Void)
}

enum CloseStep: String, Equatable { case workerRelease, hangup }

enum MiraPresence: Equatable {
    case needsPairing
    case repairRequired
    case blocked(missing: String)   // trusted, but no supported same-call route
    case readyToConnect
    case connecting
    case connected
    case closing
    case closed
    case cleanupIncomplete(Set<String>)
    var showsActiveCall: Bool { self == .connected }
}

/// Inert speaker-return path. It counts and discards; no audio is routed here.
struct SpeakerPacket: Equatable { let epoch: UInt64; let sequence: UInt64 }
final class SpeakerPath {
    private(set) var enabledEpoch: UInt64?
    private(set) var accepted = 0
    private(set) var droppedStale = 0
    private(set) var droppedDisabled = 0
    private(set) var lastSequence: UInt64 = 0
    var isEnabled: Bool { enabledEpoch != nil }
    func enable(epoch: UInt64) { enabledEpoch = epoch; lastSequence = 0 }
    func disable() { enabledEpoch = nil; lastSequence = 0 }
    func accept(_ packet: SpeakerPacket) -> Bool {
        guard let epoch = enabledEpoch else { droppedDisabled += 1; return false }
        guard packet.epoch == epoch, packet.sequence > lastSequence else { droppedStale += 1; return false }
        lastSequence = packet.sequence; accepted += 1
        return true
    }
}

final class CompanionLifecycle: ObservableObject {
    @Published private(set) var presence: MiraPresence
    @Published private(set) var muted = true            // every open starts muted
    @Published private(set) var physicalConsent = false // set only by the existing robot-consent path
    @Published private(set) var speakerEnabled = false
    private(set) var epoch: UInt64 = 0
    private(set) var closeLog: [CloseStep] = []
    let speaker = SpeakerPath()
    let destination: LaunchDestination
    private let route: SameCallRoute
    private let resources: OwnedAudioResources
    private var closeInFlight = false

    init(trust: LaunchTrustProviding = UnavailableLaunchTrust(), route: SameCallRoute = UnsupportedSameCallRoute(), resources: OwnedAudioResources) {
        self.route = route; self.resources = resources
        destination = LaunchDestination.decide(trust.current())
        switch trust.current() {
        case .absent: presence = .needsPairing
        case .stale: presence = .repairRequired
        case .valid:
            if case .unsupported(let m) = route.capability { presence = .blocked(missing: m) } else { presence = .readyToConnect }
        }
    }

    var canOpen: Bool { presence == .readyToConnect }
    /// Capture may be armed only in a confirmed call, after robot consent, while unmuted.
    var captureMayArm: Bool { presence == .connected && physicalConsent && !muted }

    /// Explicit user gesture (button). Never invoked implicitly by launch.
    func open() {
        guard canOpen, epoch < UInt64.max - 1 else { return }
        epoch += 1; muted = true; physicalConsent = false; closeLog = []
        presence = .connecting
        let mine = epoch
        route.connect(epoch: mine) { [weak self] e, event in self?.routeReported(e, event) }
    }

    func routeReported(_ e: UInt64, _ event: SameCallEvent) {
        guard e == epoch, !closeInFlight else { return }
        switch (presence, event) {
        case (.connecting, .connected): presence = .connected
        case (.connecting, .failed): presence = route.capability.isSupported ? .readyToConnect : .blocked(missing: "Connection failed")
        case (.connected, .ended), (.connected, .failed):
            // Lost call: disarm everything now; leave deliberate reopen to the user.
            muted = true; physicalConsent = false; disableSpeaker()
            close()
        default: break
        }
    }

    /// Existing robot protocol reports physical consent; this object never grants it.
    func robotConsentReported(epoch e: UInt64) {
        guard e == epoch, presence == .connected else { return }
        physicalConsent = true
    }
    func setMuted(_ value: Bool) {
        if value { muted = true; disableSpeaker(); return }
        guard presence == .connected, physicalConsent else { return }
        muted = false
    }
    func setSpeaker(_ on: Bool) {
        if !on { disableSpeaker(); return }
        guard presence == .connected, physicalConsent, !muted, route.capability.returnsAudio else { return }
        speaker.enable(epoch: epoch); speakerEnabled = true
    }
    private func disableSpeaker() { speaker.disable(); speakerEnabled = false }

    /// Window close and Quit. Idempotent. The local state is disarmed synchronously and the
    /// single atomic worker shutdown starts immediately; "closed" is reported only after the
    /// exact worker exit confirms release.
    func close(_ finished: ((Bool) -> Void)? = nil) {
        // Pairing views also own a worker, so every non-closed state runs the release.
        if presence == .closed { finished?(true); return }
        if closeInFlight { finished?(false); return }
        closeInFlight = true
        let hadCall = presence == .connecting || presence == .connected
        epoch += 1                       // late connect/packet callbacks are now stale
        let closingEpoch = epoch - 1
        muted = true; physicalConsent = false; disableSpeaker(); presence = .closing
        var issues = Set<String>()
        let finish: () -> Void = { [weak self] in
            guard let self = self else { finished?(issues.isEmpty); return }
            self.closeInFlight = false
            self.presence = issues.isEmpty ? .closed : .cleanupIncomplete(issues)
            finished?(issues.isEmpty)
        }
        resources.shutdownAndConfirmRelease { [weak self] ok in
            guard let self = self else { finish(); return }
            self.closeLog.append(.workerRelease)
            if !ok { issues.insert(CloseStep.workerRelease.rawValue) }
            if hadCall && self.route.capability.canHangUp {
                self.route.hangUp(epoch: closingEpoch) { [weak self] hungUp in
                    self?.closeLog.append(.hangup)
                    if !hungUp { issues.insert(CloseStep.hangup.rawValue) }
                    finish()
                }
            } else { finish() }
        }
    }
}
