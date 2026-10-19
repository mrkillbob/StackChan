import Foundation

// Inert fixtures only: no audio, no sockets, no processes, no network.
final class FixtureResources: OwnedAudioResources {
    var log: [String] = []
    var fail: Set<String> = []
    var deferred: [String: (Bool) -> Void] = [:]
    var holdWorker = false
    func shutdownAndConfirmRelease(_ done: @escaping (Bool) -> Void) {
        log.append("worker")
        if holdWorker { deferred["worker"] = done; return }
        done(!fail.contains("worker"))
    }
}
final class FixtureRoute: SameCallRoute, RobotLinkOpening {
    var linkReporter: ((UInt64, RobotLinkEvent) -> Void)?
    var linkOpens: [UInt64] = []
    func openRobotLink(epoch: UInt64, report: @escaping (UInt64, RobotLinkEvent) -> Void) { linkOpens.append(epoch); linkReporter = report }
    /// Robot link up and the (verified) call attached, for the pre-W1 scenarios.
    func up(_ e: UInt64) { linkReporter?(e, .connected); reporter?(e, .connected) }
    var cap: SameCallCapability
    var reporter: ((UInt64, SameCallEvent) -> Void)?
    var connects: [UInt64] = []
    var hangups: [UInt64] = []
    var hangupOK = true
    init(_ cap: SameCallCapability) { self.cap = cap }
    var capability: SameCallCapability { cap }
    func connect(epoch: UInt64, report: @escaping (UInt64, SameCallEvent) -> Void) { connects.append(epoch); reporter = report }
    func hangUp(epoch: UInt64, completion: @escaping (Bool) -> Void) { hangups.append(epoch); completion(hangupOK) }
}
struct FixtureTrust: LaunchTrustProviding { let value: LaunchTrust; func current() -> LaunchTrust { value } }

func check(_ ok: Bool, _ what: String, line: Int = #line) {
    if !ok { FileHandle.standardError.write(Data("FAIL line \(line): \(what)\n".utf8)); exit(1) }
}
let supported = SameCallCapability.supported(routeID: "fixture", hangup: true, returnAudio: true)

@main struct MiraLifecycleChecks {
    static func main() {
        // Production .paired is a busy worker session, not a persisted trust receipt.
        let pairedHome = MiraHomeSession(phase: .paired, cleanupUnconfirmed: false)
        check(pairedHome.pairedThisSession && pairedHome.showsControls, "paired home retains session controls")
        check(MiraHomeSession(phase: .restoring, cleanupUnconfirmed: false).showsControls, "restoring retains controls")
        check(MiraHomeSession(phase: .failed, cleanupUnconfirmed: true).showsControls, "unconfirmed terminal cleanup stays visible")
        let endedHome = MiraHomeSession(phase: .closed, cleanupUnconfirmed: false)
        check(!endedHome.pairedThisSession && !endedHome.showsControls, "ended session grants no remembered pairing")
        // Launch gate
        check(LaunchDestination.decide(.absent) == .miraLanding, "absent trust still lands on Mira home")
        check(LaunchDestination.decide(.stale) == .miraLanding, "stale trust still lands on Mira home")
        check(LaunchDestination.decide(.valid) == .miraLanding, "valid trust -> Mira")
        check(UnavailableLaunchTrust().current() == .absent, "production trust is absent")
        let none = CompanionLifecycle(resources: FixtureResources())
        check(none.presence == .needsPairing && !none.canOpen, "default production lifecycle never lands or connects")
        let stale = CompanionLifecycle(trust: FixtureTrust(value: .stale), route: FixtureRoute(supported), resources: FixtureResources())
        check(stale.presence == .repairRequired && !stale.canOpen, "stale trust cannot open")
        stale.open(); check(stale.presence == .repairRequired, "open refused on stale trust")

        // Honest blocked: valid trust, production route -> never connected
        let blocked = CompanionLifecycle(trust: FixtureTrust(value: .valid), resources: FixtureResources())
        if case .blocked(let m) = blocked.presence { check(m.contains("existing Mira call"), "missing capability named") } else { check(false, "blocked") }
        blocked.open(); check(!blocked.presence.showsActiveCall && blocked.epoch == 0, "blocked open does nothing")
        let unsupportedRoute = UnsupportedSameCallRoute()
        unsupportedRoute.connect(epoch: 1) { _, _ in check(false, "unsupported route must not report") }
        check(!unsupportedRoute.capability.isSupported && !unsupportedRoute.capability.returnsAudio, "unsupported capability")

        // Connect: only a route report makes it connected; starts muted
        let res = FixtureResources(), route = FixtureRoute(supported)
        let life = CompanionLifecycle(trust: FixtureTrust(value: .valid), route: route, link: route, resources: res)
        check(life.presence == .readyToConnect, "ready")
        life.open(); check(life.presence == .connecting && !life.presence.showsActiveCall && life.muted, "connecting is not connected; muted")
        check(route.connects == [1], "one connect attempt")
        life.open(); check(route.connects == [1], "no double open")
        route.reporter?(99, .connected); check(life.presence == .connecting, "wrong epoch ignored")
        route.up(1); check(life.presence == .connected && life.presence.showsActiveCall, "connected after report")
        check(!life.captureMayArm, "no arm without consent/unmute")
        life.setMuted(false); check(life.muted, "cannot unmute before robot consent")
        life.setSpeaker(true); check(!life.speakerEnabled, "speaker needs consent")
        life.robotConsentReported(epoch: 7); check(!life.physicalConsent, "stale consent ignored")
        life.robotConsentReported(epoch: 1); check(life.physicalConsent && life.muted && !life.captureMayArm, "consent alone leaves it muted")
        life.setMuted(false); check(life.captureMayArm, "arm only after consent + unmute")
        life.setSpeaker(true); check(life.speakerEnabled, "speaker explicit")
        check(life.speaker.accept(SpeakerPacket(epoch: 1, sequence: 1)), "current packet accepted")
        check(!life.speaker.accept(SpeakerPacket(epoch: 1, sequence: 1)), "replayed sequence dropped")
        check(!life.speaker.accept(SpeakerPacket(epoch: 0, sequence: 5)), "stale epoch dropped")
        life.setMuted(true); check(!life.speakerEnabled && !life.captureMayArm, "mute disarms capture and speaker")
        check(!life.speaker.accept(SpeakerPacket(epoch: 1, sequence: 2)) && life.speaker.droppedDisabled == 1, "muted speaker drops")
        life.setMuted(false); check(!life.speakerEnabled, "unmute does not re-enable speaker")

        // Clean close ordering and hangup
        var finished: Bool?
        life.close { finished = $0 }
        check(res.log == ["worker"], "single atomic worker shutdown \(res.log)")
        check(route.hangups == [1], "hangup requested for the exact epoch")
        check(life.closeLog.last == .hangup && finished == true && life.presence == .closed, "closed")
        check(life.muted && !life.physicalConsent && !life.captureMayArm, "all disarmed after close")
        route.up(1); check(life.presence == .closed, "late callback after close ignored")
        life.open(); check(life.presence == .closed, "no reopen without relaunch")
        life.close { finished = $0 }; check(finished == true && res.log.count == 1, "close idempotent")

        // Startup cancellation: close while connecting, no hangup of non-call? (attempt is cancelled and hung up)
        let r2 = FixtureResources(), rt2 = FixtureRoute(supported)
        let l2 = CompanionLifecycle(trust: FixtureTrust(value: .valid), route: rt2, link: rt2, resources: r2)
        l2.open(); l2.close()
        rt2.up(1)
        check(l2.presence == .closed && !l2.presence.showsActiveCall, "late connect after cancel never shows connected")
        check(rt2.hangups == [1], "pending attempt cancelled via hangup")

        // No hangup capability: still releases locally, never claims hangup
        let r3 = FixtureResources(), rt3 = FixtureRoute(.supported(routeID: "x", hangup: false, returnAudio: false))
        let l3 = CompanionLifecycle(trust: FixtureTrust(value: .valid), route: rt3, link: rt3, resources: r3)
        l3.open(); rt3.up(1); l3.robotConsentReported(epoch: 1); l3.setMuted(false); l3.setSpeaker(true)
        check(!l3.speakerEnabled, "no speaker without supported return audio")
        l3.close(); check(rt3.hangups.isEmpty && l3.presence == .closed && !l3.closeLog.contains(.hangup), "no hangup without capability")

        // Failed cleanup continues remaining steps and is not reported closed
        let r4 = FixtureResources(); r4.fail = ["worker"]
        let rt4 = FixtureRoute(supported), l4 = CompanionLifecycle(trust: FixtureTrust(value: .valid), route: rt4, link: rt4, resources: r4)
        l4.open(); rt4.up(1)
        var f4: Bool?; l4.close { f4 = $0 }
        check(r4.log == ["worker"], "worker shutdown ran")
        check(f4 == false && l4.presence == .cleanupIncomplete(["workerRelease"]), "unconfirmed worker release is surfaced")
        check(rt4.hangups == [1], "hangup still requested")
        let r5 = FixtureResources(), rt5 = FixtureRoute(supported); rt5.hangupOK = false
        let l5 = CompanionLifecycle(trust: FixtureTrust(value: .valid), route: rt5, link: rt5, resources: r5)
        l5.open(); rt5.up(1); l5.close()
        check(l5.presence == .cleanupIncomplete(["hangup"]), "failed hangup surfaced")

        // Close while worker close pending: second close does not duplicate; not closed until done
        let r6 = FixtureResources(); r6.holdWorker = true
        let rt6 = FixtureRoute(supported), l6 = CompanionLifecycle(trust: FixtureTrust(value: .valid), route: rt6, link: rt6, resources: r6)
        l6.open(); rt6.up(1)
        var f6: Bool?; l6.close { f6 = $0 }
        check(r6.log == ["worker"], "shutdown starts immediately on close")
        check(l6.presence == .closing && f6 == nil && l6.muted, "closing until worker confirms")
        var again: Bool?; l6.close { again = $0 }
        check(again == false && r6.log.count == 1, "overlapping close refused")
        r6.deferred["worker"]?(true)
        check(l6.presence == .closed && f6 == true, "closed after worker confirmation")

        // Lost connection: disarms and releases, no active indicator
        let r7 = FixtureResources(), rt7 = FixtureRoute(supported)
        let l7 = CompanionLifecycle(trust: FixtureTrust(value: .valid), route: rt7, link: rt7, resources: r7)
        l7.open(); rt7.up(1); l7.robotConsentReported(epoch: 1); l7.setMuted(false)
        rt7.reporter?(1, .ended)
        check(l7.presence == .closed && l7.muted && !l7.captureMayArm && r7.log == ["worker"], "lost call closes and disarms")

        // Failed connect returns to ready for deliberate retry
        let rt8 = FixtureRoute(supported), l8 = CompanionLifecycle(trust: FixtureTrust(value: .valid), route: rt8, link: rt8, resources: FixtureResources())
        l8.open(); rt8.reporter?(1, .failed); check(l8.presence == .readyToConnect, "failed connect -> ready, not connected")
        l8.open(); check(rt8.connects == [1, 2], "fresh epoch per open")

        // Close before anything opened (pairing view) is a no-op success
        let r9 = FixtureResources(), l9 = CompanionLifecycle(resources: r9); var f9: Bool?; l9.close { f9 = $0 }
        check(f9 == true && r9.log == ["worker"], "pairing view close still releases the worker")
        print("PASS Mira lifecycle: launch gate, honest blocked state, explicit open, consent/mute, epoch-safe speaker, ordered close, startup cancel, lost call, failed cleanup; inert fixtures only")
    }
}
