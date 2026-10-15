# Selectable Stock AI adapter

The launcher opens `MakeApp()` as a stopped preview. It calls
`ConsumeStartRequest()` after each shell update. On true it must uninstall all
Mooncake apps, destroy Mooncake, and call the non-returning `RunStockAgent()`.
Opening this app stays stopped. A power-on/reset boot starts Stock only after a new explicit General Confirm opt-in; legacy boot_ai alone is ignored. Software resets, including Stop/Home, suppress autostart.

`ConsumePreviewRequest()` returns true once on an intentional software restart
from the shared-network preview; open the installed Stock app in that case,
without invoking `RunStockAgent()`. That preview still needs another Start.
The RTC cookie carries only a stopped preview request, never session consent.

Stock acquires `AudioOwner::Stock` before touching the codec. Mira must have
successfully closed its resources before launching Stock. The stock session's
always-visible top-layer Stop / Home de-energizes motor/laser/RGB outputs and uses MCU restart because upstream
has no joined, repeatable teardown. No active state survives that reset.

Stock retains official hotspot/browser Wi-Fi configuration and activation-code
flows. Existing Wi-Fi and identity are read from NVS. Phone-entered Wi-Fi and
returned MQTT/WebSocket settings are RAM-only for the current session; saved
NVS is never erased or overwritten by this adapter. A missing UUID is generated
in RAM only after Start. Phone account/device activation stays with the official
vendor app; this firmware does not collect account credentials.

The explicitly started Stock session now restores original camera, motor, head-pet, IMU, speaking/idle reactions, MCP tools and reminder behavior through Hal.startXiaozhi. The stopped selector and Companion do not activate those capabilities. The vendor idle state enables its local wake word;
the status explicitly says so. StopListening is not advertised as microphone mute.

`generate_backend.py` deterministically adapts pinned Xiaozhi commit
`e77dedb1309153bb63fed285772962c920c97dd4` and reviewed esp-wifi-connect 3.1.1
files into the build directory. Hotspot advanced-settings writes are rejected;
Stock-session Wi-Fi setup remains RAM-only. The dedicated Wi-Fi Setup app/Setup worker instead persists successfully tested choices to a bounded versioned CRC-protected atomic NVS blob; legacy keys remain a fallback. Both Stock and Companion read the same saved SSID manager. Acoustic and BLUFI build modes are rejected
because this adapter supports the hotspot/browser flow only. SHA-256 pins fail closed on source drift.
The surrounding official StackChan reference is
`1b5765599fba8aaad1811d9a79358ccc7051f5f3`.
Generated variants suppress asset downloads, firmware upgrade code and user-only
update tools; the vendor OTA endpoint is contacted solely for activation and
protocol configuration, not firmware download or installation.

Source generation can be checked without building or device access:

```sh
python main/apps/stock_agent/generate_backend.py \
  --xiaozhi xiaozhi-esp32/main \
  --wifi managed_components/78__esp-wifi-connect \
  --output /tmp/stock-agent-backend-check
```

On-device pairing, audio, network and UI behavior require separately authorized
hardware validation. No such validation is claimed by the source adapter.
