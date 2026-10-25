import Foundation

/// Source adapter for a future explicitly admitted exclusive video pipe reader.
/// Construction opens no descriptor, starts no reader and enables no extension.
/// The current process-generation owner supplies `current`; a transport boolean
/// or an untrusted event must never implement it. Existing pipeline authority,
/// capture clock and extension approval remain required for every frame.
final class WebcamOwnedPipeAdapter {
    private let decoder: WebcamWorkerLineDecoder

    init(pipeline: CompanionWebcamPipeline, current: @escaping () -> Bool,
         cleanupPending: @escaping () -> Void) {
        decoder = WebcamWorkerLineDecoder(current: current,
            consume: { pipeline.handleExistingWorkerEvent($0) },
            requestStop: {
                if !pipeline.revoke() { cleanupPending() }
            })
    }

    @discardableResult func acceptOwnedRead(_ bytes: Data) -> Bool { decoder.feed(bytes) }
    @discardableResult func ownedEOF() -> Bool { decoder.finish() }
    func ownerLost() { decoder.revoke() }
}
