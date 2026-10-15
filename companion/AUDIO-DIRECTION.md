# Public audio device direction remains blocked

The user's observed defect is accurate: BOTH StackChan names appear in BOTH
input and output lists. Each current aggregate exposes two input channels, two
output channels, one input stream and one output stream. Both default-device
eligibility flags are1. This is duplicated bidirectional exposure, not swapped
labels or a demonstrated reversal of signal flow.

The internal buses intentionally remain duplex. The native worker writes robot
microphone PCM to2-A's OUTPUT side so Mac clients can read2-A's INPUT side. It
reads Mac playback from2-B's INPUT side after clients write to2-B's OUTPUT side.
Do not swap those UIDs or break their opposite-side loopback transport.

A private metadata-only probe requested microphone channels2/0 and speaker0/2
using the SDK's subdevice channel-count dictionary keys. Both still exposed2/2
channels and1/1 scoped streams. Both temporary private aggregates were destroyed
successfully. The SDK describes these keys as subdevice state, not a documented
channel mask. A separate read-only query reports the four existing alias/scope
CanBeDefaultDevice properties are not settable. No setter, capture, playback,
permission change or default-route write was attempted.

The source mitigation inspects actual scope/channel/stream enumeration, rejects
the duplex topology for both public roles, and stops recreating the known-wrong
aliases. The app reports that dedicated directional devices are unavailable;
Sync's internal transport is unchanged. AudioEndpointDirectionChecks covers
correct input-only/output-only shapes, the actual duplex counterexample,
reversed scopes, extra opposite streams and failed/incomplete metadata.

This does not fix current OS enumeration or install new endpoints. A distinct
supported device/backing implementation is required. A HAL device implementation
would be a new integration and installation scope; process taps or system/private
API tricks have not been substituted. Its permissions, lifecycle, transport
mapping and actual signal path need review before replacing existing devices.
No universal claim that macOS direction filtering is impossible is made here.

Installation remains held on this requirement. The firmware/source tests and
opaque backup do not establish live audio acceptance. Any actual robot audio
check still requires the existing authenticated session and physical Listen.
