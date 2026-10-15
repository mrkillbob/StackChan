import Foundation
import Darwin
@main struct LifecycleChecks {
 static func pump(_ seconds: Double = 3, _ condition: () -> Bool) {
  let deadline = Date().addingTimeInterval(seconds)
  while !condition() && Date() < deadline { RunLoop.current.run(until: Date().addingTimeInterval(0.02)) }
  precondition(condition(), "bounded fixture did not complete")
 }
 static func fake(_ code: String) -> Process {
  let p=Process();p.executableURL=URL(fileURLWithPath:"/opt/homebrew/bin/python3.14")
  p.arguments=["-B","-u","-c",FixtureCustody.wrap(code)]
  return p
 }
 static func main() {
  let root=URL(fileURLWithPath:"/tmp")
  let inert=PairingController(approved:false,root:root)
  assert(!inert.canPrimary && !inert.canCancel && !inert.canConfirm && !inert.audioApproved)
  let scoped=PairingController(approved:true,root:root,audioApproved:true)
  assert(scoped.audioApproved)
  let denied=PairingController(approved:false,root:root,audioApproved:true)
  assert(!denied.audioApproved)
  let native = PairingController(approved:true,root:root,audioApproved:true,audioMode:.nativeConversation)
  assert(native.audioDisclosure.contains("original dot voice call"))
  assert(native.audioDisclosure.contains("call's audio plays on StackChan"))
  assert(!native.audioDisclosure.contains("discarded"))
  assert(native.audioSubtitle.contains("Original dot call"))
  assert(Array(native.workerLaunchArguments.suffix(3)) == ["--approved-audio","--audio-mode","native_conversation"])
  assert(scoped.audioDisclosure.contains("counted and discarded locally"))
  assert(scoped.audioDisclosure.contains("No audio recording or forwarding"))
  assert(Array(scoped.workerLaunchArguments.suffix(3)) == ["--approved-audio","--audio-mode","transport_test"])
  assert(!inert.workerLaunchArguments.contains("--approved-audio"))
  let normal="""
import sys,json
for line in sys.stdin:
 c=json.loads(line)['command']
 if c in ('prepare','retry'):print(json.dumps({'state':'comparing','code':'012345','remaining':120}),flush=True)
 elif c=='confirm':print(json.dumps({'state':'paired','acknowledged':True}),flush=True)
 elif c=='cancel':print(json.dumps({'state':'cancelled'}),flush=True)
"""
  let a=PairingController(approved:true,root:root,processFactory:{fake(normal)},oneAttempt:false)
  a.primary();assert(!a.canPrimary);pump{a.phase == .comparing};assert(a.code=="012345" && a.canConfirm)
  assert(a.secondsRemaining(at:ProcessInfo.processInfo.systemUptime+121)==0)
  a.primary();assert(a.confirmationSent && !a.canConfirm);pump{a.phase == .paired};assert(!a.canPrimary)
  a.cancel();assert(a.cleanupPending && !a.canPrimary);pump{a.phase == .cancelled};assert(a.canPrimary)
  a.primary();pump{a.phase == .comparing};assert(!a.confirmationSent)
  var stopped=false;a.shutdown{closed in stopped=closed};pump{stopped}
  let audio=PairingController(approved:true,root:root,processFactory:{fake(normal)},audioApproved:true)
  audio.primary();pump{audio.phase == .comparing}
  assert(audio.primaryTitle == "Confirm & connect")
  assert(audio.instruction.contains("temporary") && audio.instruction.contains("microphone") && audio.instruction.contains("speaker"))
  audio.primary();pump{audio.phase == .paired}
  assert(!audio.instruction.contains("tap Connect") && !audio.instruction.contains("then Allow"))
  var audioStopped=false;audio.shutdown{closed in audioStopped=closed};pump{audioStopped}
  let single=PairingController(approved:true,root:root,processFactory:{fake(normal)})
  single.primary();pump{single.phase == .comparing};single.cancel();pump{single.phase == .cancelled}
  assert(!single.canPrimary && single.primaryTitle == "New approval needed")
  single.primary();assert(single.phase == .cancelled)
  var singleStopped=false;single.shutdown{closed in singleStopped=closed};pump{singleStopped}
  let cleanup="""
import sys,json
sys.stdin.readline()
print(json.dumps({'state':'failed','message':'cleanup_unconfirmed','cleanup':['audio','grant','usb']}),flush=True)
"""
  let blocked=PairingController(approved:true,root:root,processFactory:{fake(cleanup)},oneAttempt:false)
  blocked.primary();pump{blocked.phase == .failed && blocked.cleanupIssues == [.audio,.grant,.usb]}
  assert(!blocked.canPrimary && blocked.primaryTitle == "Cleanup needed")
  assert(blocked.instruction.contains("Do not try again") && blocked.instruction.contains("microphone and speaker routes are restored"))
  assert(blocked.instruction.contains("pairing grant was revoked") && blocked.instruction.contains("USB pairing session is closed"))
  var blockedStopped=false;blocked.shutdown{closed in blockedStopped=closed};pump{blockedStopped}
  let invalid="""
import sys,json,time
sys.stdin.readline()
print(json.dumps({'state':'failed','credential':'synthetic-no-authority'}),flush=True)
print(json.dumps({'state':'cancelled'}),flush=True)
for line in sys.stdin:pass
time.sleep(1)
"""
  let b=PairingController(approved:true,root:root,processFactory:{fake(invalid)},oneAttempt:false)
  b.primary();pump{b.phase == .failed && b.cleanupPending};assert(!b.canPrimary)
  pump{!b.cleanupPending};assert(b.canPrimary)
  var count=0
  let partial="""
import sys
sys.stdin.readline()
sys.stdout.write('{"state":"comparing",');sys.stdout.flush()
"""
  let c=PairingController(approved:true,root:root,processFactory:{count+=1;return fake(count==1 ? partial : normal)},oneAttempt:false)
  c.primary();pump{c.phase == .failed && !c.cleanupPending};c.primary();pump{c.phase == .comparing};assert(c.code=="012345")
  var freshStopped=false;c.shutdown{closed in freshStopped=closed};pump{freshStopped}
  let stubborn="""
import sys,signal,time,json
signal.signal(signal.SIGTERM,signal.SIG_IGN)
sys.stdin.readline();print(json.dumps({'state':'waiting_robot'}),flush=True)
while True:time.sleep(0.1)
"""
  let d=PairingController(approved:true,root:root,processFactory:{fake(stubborn)})
  d.primary();pump{d.phase == .waiting_robot};var stubbornClosed=false;d.shutdown{closed in stubbornClosed=closed}
  pump(9){stubbornClosed};assert(!d.cleanupPending)
  print("PASS nativeUI real-pipe fixtures: preview, repeatedclick, confirm, cancel/retry, malformedstatus cleanup, stalepartial isolation, EOF closure and TERM-resistant workerKILL; nohardware/network")
 }
}
