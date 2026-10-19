import SwiftUI

/// One strip, three separate facts: the robot's microphone, the Mac bridge, and the call's own
/// mute (which Companion cannot read). Half-duplex: the mic pauses while StackChan speaks.
struct MicrophoneStripView: View {
    let strip: MicrophoneStrip
    var body: some View {
        VStack(alignment: .leading, spacing: 4) {
            ForEach(strip.lines, id: \.self) { Text($0).font(.caption) }
            Text("The microphone pauses while StackChan plays sound (half-duplex).").font(.caption2).foregroundStyle(.secondary)
        }
        .padding(8).frame(maxWidth: .infinity, alignment: .leading)
        .background(RoundedRectangle(cornerRadius: 6).fill(Color.secondary.opacity(0.12)))
        .accessibilityElement(children: .combine)
    }
}
