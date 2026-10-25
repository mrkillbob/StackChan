import Foundation

var checks = 0
func check(_ predicate: @autoclosure () -> Bool) {
    precondition(predicate(), "decoder fixture failed at check \(checks + 1)")
    checks += 1
}
func bytes(_ text: String) -> Data { Data(text.utf8) }

// Deliberately mocked boundary: these tests do not run a CMIO pipeline.
final class CompanionWebcamPipeline {
    var received = [Data]()
    var revocations = 0
    func handleExistingWorkerEvent(_ line: Data) -> Bool { received.append(line); return true }
    func revoke() -> Bool { revocations += 1; return false }
}

do { // Fragmentation and coalescing; callbacks retain separate complete events.
    var lines = [Data](); var stops = 0
    let decoder = WebcamWorkerLineDecoder(current: { true }, consume: { lines.append($0); return true }, requestStop: { stops += 1 })
    check(decoder.feed(bytes("{\"type\":")))
    check(lines.isEmpty)
    check(decoder.feed(bytes("\"webcam_off\"}\n{\"type\":\"webcam_off\"}\n")))
    check(lines.count == 2)
    check(lines[0] == lines[1])
    check(decoder.finish())
    check(stops == 1)
    check(!decoder.finish())
    check(!decoder.feed(bytes("later\n")))
    check(stops == 1)
}
do { // Exact line limit, including newline.
    var sizes = [Int](); var stops = 0
    let decoder = WebcamWorkerLineDecoder(current: { true }, consume: { sizes.append($0.count); return true }, requestStop: { stops += 1 })
    check(decoder.feed(Data(repeating: 120, count: 4096)))
    check(decoder.feed(Data(repeating: 120, count: 4095)))
    check(decoder.feed(bytes("\n")))
    check(sizes == [8191])
    check(decoder.finish())
    check(stops == 1)
}
do { // Overlong unfinished line never reaches consumer.
    var received = 0; var stops = 0
    let decoder = WebcamWorkerLineDecoder(current: { true }, consume: { _ in received += 1; return true }, requestStop: { stops += 1 })
    check(decoder.feed(Data(repeating: 120, count: 4096)))
    check(!decoder.feed(Data(repeating: 120, count: 4096)))
    check(received == 0 && stops == 1)
}
do { // Partial EOF is rejected; no manufactured newline.
    var received = 0; var stops = 0
    let decoder = WebcamWorkerLineDecoder(current: { true }, consume: { _ in received += 1; return true }, requestStop: { stops += 1 })
    check(decoder.feed(bytes("{\"type\":\"webcam_fragment\"")))
    check(!decoder.finish())
    check(received == 0 && stops == 1)
}
do { // Owner changes during callback: second event cannot enter replacement.
    var valid = true; var received = 0; var stops = 0
    let decoder = WebcamWorkerLineDecoder(current: { valid }, consume: { _ in received += 1; valid = false; return true }, requestStop: { stops += 1 })
    check(!decoder.feed(bytes("first\nsecond\n")))
    check(received == 1 && stops == 1)
}
do { // Wrong generation at ingress.
    var received = 0; var stops = 0
    let decoder = WebcamWorkerLineDecoder(current: { false }, consume: { _ in received += 1; return true }, requestStop: { stops += 1 })
    check(!decoder.feed(bytes("first\n")))
    check(received == 0 && stops == 1)
}
do { // Consumer failure is terminal, even with complete following events.
    var received = 0; var stops = 0
    let decoder = WebcamWorkerLineDecoder(current: { true }, consume: { _ in received += 1; return false }, requestStop: { stops += 1 })
    check(!decoder.feed(bytes("first\nsecond\n")))
    check(received == 1 && stops == 1)
}
do { // Oversized chunk, including many short lines, is bounded at ingress.
    var received = 0; var stops = 0
    let decoder = WebcamWorkerLineDecoder(current: { true }, consume: { _ in received += 1; return true }, requestStop: { stops += 1 })
    check(!decoder.feed(Data(repeating: 10, count: 4097)))
    check(received == 0 && stops == 1)
}
do { // Reentrant reader callbacks cannot reorder or share partial state.
    var decoder: WebcamWorkerLineDecoder!; var stops = 0
    decoder = WebcamWorkerLineDecoder(current: { true }, consume: { _ in
        check(!decoder.feed(bytes("nested\n"))); return true
    }, requestStop: { stops += 1 })
    check(!decoder.feed(bytes("outer\n")))
    check(stops == 1)
}
do {
    let pipeline = CompanionWebcamPipeline(); var pending = 0
    let adapter = WebcamOwnedPipeAdapter(pipeline: pipeline, current: { true }, cleanupPending: { pending += 1 })
    check(adapter.acceptOwnedRead(bytes("{\"type\":\"webcam_off\"}\n")))
    check(pipeline.received.count == 1)
    check(adapter.ownedEOF())
    adapter.ownerLost()
    check(pipeline.revocations == 1 && pending == 1)
}
print("\(checks) decoder/adapter checks passed; no device/CMIO activation")
