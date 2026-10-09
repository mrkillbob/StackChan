import Foundation
@main struct LifecycleChecks {
 static func pump(_ seconds: Double = 3, _ condition: () -> Bool) {
  let deadline = Date().addingTimeInterval(seconds)
  while !condition() && Date() < deadline { RunLoop.current.run(until: Date().addingTimeInterval(0.02)) }
  precondition(condition(), "bounded fixture did not complete")
 }
 static func fake(_ code: String) -> Process {
  let p=Process();p.executableURL=URL(fileURLWithPath:"/usr/bin/python3");p.arguments=["-u","-c",code];return p
 }
 static func main() {
  let root=URL(fileURLWithPath:"/tmp")
  let inert=PairingController(approved:false,root:root)
  assert(!inert.canPrimary && !inert.canCancel && !inert.canConfirm)
  let normal="""
import sys,json
for line in sys.stdin:
 c=json.loads(line)['command']
 if c in ('prepare','retry'):print(json.dumps({'state':'comparing','code':'012345','remaining':120}),flush=True)
 elif c=='confirm':print(json.dumps({'state':'paired','acknowledged':True}),flush=True)
 elif c=='cancel':print(json.dumps({'state':'cancelled'}),flush=True)
"""
  let a=PairingController(approved:true,root:root,processFactory:{fake(normal)})
  a.primary();assert(!a.canPrimary);pump{a.phase == .comparing};assert(a.code=="012345" && a.canConfirm)
  assert(a.secondsRemaining(at:ProcessInfo.processInfo.systemUptime+121)==0)
  a.primary();assert(a.confirmationSent && !a.canConfirm);pump{a.phase == .paired};assert(!a.canPrimary)
  a.cancel();assert(a.cleanupPending && !a.canPrimary);pump{a.phase == .cancelled};assert(a.canPrimary)
  a.primary();pump{a.phase == .comparing};assert(!a.confirmationSent)
  var stopped=false;a.shutdown{closed in stopped=closed};pump{stopped}
  let invalid="""
import sys,json,time
sys.stdin.readline()
print(json.dumps({'state':'failed','credential':'synthetic-no-authority'}),flush=True)
print(json.dumps({'state':'cancelled'}),flush=True)
for line in sys.stdin:pass
time.sleep(1)
"""
  let b=PairingController(approved:true,root:root,processFactory:{fake(invalid)})
  b.primary();pump{b.phase == .failed && b.cleanupPending};assert(!b.canPrimary)
  pump{!b.cleanupPending};assert(b.canPrimary)
  var count=0
  let partial="""
import sys
sys.stdin.readline()
sys.stdout.write('{"state":"comparing",');sys.stdout.flush()
"""
  let c=PairingController(approved:true,root:root,processFactory:{count+=1;return fake(count==1 ? partial : normal)})
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
