import Foundation
import Darwin
final class MockRestorationVerifier: SessionRestorationVerifying {
 var allow=false;var wrongID=false
 func receipt(for id: UUID) -> RestorationReceipt? {
  guard allow else { return nil }
  return RestorationReceipt(attemptID:wrongID ? UUID() : id, originalRoutesRestored:true,ownedProcessesClosed:true,usbReleased:true,listenerReleased:true)
 }
}
@main struct CompanionChecks {
 static func pump(_ condition: () -> Bool) {
  let deadline=Date().addingTimeInterval(3)
  while !condition() && Date()<deadline { RunLoop.current.run(until:Date().addingTimeInterval(0.01)) }
  precondition(condition(),"bounded companion fixture did not complete")
 }
 static func fake(_ body: String) -> Process {
  let process=Process();process.executableURL=URL(fileURLWithPath:"/opt/homebrew/bin/python3.14")
  process.arguments=["-B","-u","-c",FixtureCustody.wrap(body)]
  return process
 }
 static func main() {
  let root=URL(fileURLWithPath:"/tmp");let verifier=MockRestorationVerifier();var launches=0
  let flow="""
import sys,json,time
for line in sys.stdin:
 c=json.loads(line)['command']
 if c=='prepare':print(json.dumps({'state':'comparing','code':'012345'}),flush=True)
 elif c=='confirm':
  print(json.dumps({'state':'paired','acknowledged':True,'activity':'physical_consent'}),flush=True)
  time.sleep(.08)
  print(json.dumps({'state':'paired','acknowledged':True,'activity':'conversation','audio_remaining':60}),flush=True)
 elif c=='cancel':
  print(json.dumps({'state':'restoring'}),flush=True)
  time.sleep(.05)
  print(json.dumps({'state':'closed','message':'audio_complete'}),flush=True)
"""
  let native=PairingController(approved:true,root:root,processFactory:{launches+=1;return fake(flow)},audioApproved:true,audioMode:.nativeConversation,restorationVerifier:verifier)
  assert(native.stage == .callRequired && !native.canPrimary)
  native.primary();assert(launches==0)
  native.acknowledgeOriginalCall();assert(native.readyNow && native.canPrimary && launches==0)
  native.primary();pump{native.phase == .comparing};assert(native.stage == .pairing)
  native.primary();pump{native.activity == .physicalConsent};assert(native.stage == .physicalConsent)
  pump{native.activity == .conversation};assert(native.stage == .conversation && native.secondsRemaining() != nil)
  native.cancel();pump{native.phase == .restoring};assert(native.stage == .restoring && !native.canPrimary)
  pump{native.phase == .closed};assert(native.stage == .restoring)
  var closed=false;native.shutdown{closed=$0};pump{closed};assert(native.stage == .restoring)
  verifier.allow=true;verifier.wrongID=true;native.refreshRestoration();assert(native.stage == .restoring)
  verifier.wrongID=false;native.refreshRestoration();assert(native.stage == .closed && !native.canPrimary)
  native.setRememberRequested(true);assert(native.rememberRequested && native.rememberFeedback == .requested)
  native.forgetDevice();assert(!native.rememberRequested && native.rememberFeedback == .preferenceCleared)
  let failure="""
import sys,json
sys.stdin.readline()
print(json.dumps({'state':'failed','message':'certificate_expired'}),flush=True)
print(json.dumps({'state':'closed','message':'closed'}),flush=True)
"""
  let expired=PairingController(approved:true,root:root,processFactory:{fake(failure)},oneAttempt:false)
  expired.primary();pump{expired.phase.terminal};assert(expired.recovery == .trustExpired)
  assert(expired.instruction.contains("trust profile has expired"))
  var stopped=false;expired.shutdown{stopped=$0};pump{stopped}
  assert(expired.recovery == .trustExpired)
  let disconnected="""
import sys,json,os,time,signal
signal.signal(signal.SIGTERM,signal.SIG_IGN)
sys.stdin.readline()
os.close(0)
print(json.dumps({'state':'comparing','code':'012345'}),flush=True)
time.sleep(30)
"""
  let broken=PairingController(approved:true,root:root,processFactory:{fake(disconnected)},oneAttempt:false)
  broken.primary();pump{broken.phase == .comparing}
  broken.primary();assert(broken.phase == .failed && broken.recovery == .connection && !broken.canPrimary)
  var brokenStopped=false;broken.shutdown{brokenStopped=$0}
  // The controller escalates at6s and reports uncertainty at8s; allow that policy to finish.
  let deadline=Date().addingTimeInterval(9)
  while !brokenStopped && Date()<deadline { RunLoop.current.run(until:Date().addingTimeInterval(0.01)) }
  assert(brokenStopped && !broken.cleanupPending)
  print("PASS Companion manual call, ready cue, paired/physical-consent/conversation, restoring, session-bound closure, first recovery, closed-input cleanup and unsaved opt-in; no hardware/audio/network")
 }
}
