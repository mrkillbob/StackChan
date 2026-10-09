# Stock feature restoration and acceptance

Official reference: M5Stack StackChan `1b5765599fba8aaad1811d9a79358ccc7051f5f3` (firmware 1.5.1). The reported regression was reproduced by source inspection of flashed image `000a245183e288df4cafdcefb4af16892c8ebef3efc933c7fcb64b7c4176c4f3`, built from `1cedafdad113b3fb501b4ee5cc6e84b0671a427d`.

Cause: custom Setup replaced every callback except brightness/volume with LockedFeatureWorker. Global read-only Settings denied Confirm persistence; the generated SsidManager discarded saves. Several launcher entries were placeholders. This was a custom-firmware regression.

The restoration uses real original classes/workers and an explicit-start session before radios, microphone, camera or motors. Original services without joined shutdown return through MCU reset; motor VM_EN is explicitly switched off before exit and again during stopped boot. The board's actual off state still requires measurement/device validation. Software resets suppress saved AI autostart so Stop/Home remains usable. General Confirm supplies a new opt-in before power-on/reset boot can autostart AI; old saved boot_ai alone is insufficient.

Successful Wi-Fi choices use one versioned CRC-protected wifi/config_v1 NVS blob. Legacy Wi-Fi keys remain untouched as fallback; account/calibration namespaces remain unchanged by provisioning. Both native and companion saved-network readers use the same SSID manager. A failed association does not save credentials; backend failures remain visible instead of returning Home as though saved. No Wi-Fi secrets belong in logs, receipts or chat.

## Evidence boundaries

Source restoration means the real implementation is reachable. Host checks exercise bounded credential encoding/decoding, tamper/truncation rejection, gated writes, commit failure, legacy-key preservation, scoped setting writes and existing Tools/game algorithms. An ESP-IDF compile establishes compilation/linking, not successful radio, cloud, servo or microphone operation. Every restored feature below still needs device acceptance on the replacement image. Prior menu/flash acceptance does not establish it.

| Original feature | Source implementation | Software check | Required device acceptance |
|---|---|---|---|
| Launcher/icons/carousel/screensaver | Original launcher, distinct installed app names | Source review/build | Scroll every entry; return repeatedly; 30s idle wake; no unintended hardware activity |
| Initial setup | Real phone Wi-Fi worker reachable from Setup/Wi-Fi Setup; automatic boot wizard deferred | Source review/build | New/unconfigured device starts setup manually; Skip/Back/Next behave; hardware test available separately |
| Change Wi-Fi | Actual original phone BLE setup plus official browser AP; scoped durable save after connection | Credential/store host checks; source review/build | Phone app handshake, select another network, reboot and reconnect; no-network, wrong password, retry, open/32-byte SSID, cancel; saved network used by Companion/App Center/Stock AI |
| Phone binding/configured status | Original handshake and app_config/is_configed scope | Settings host check; source review/build | StackChan World discovers/connects; ID/status and successful configured state survive reboot |
| Brightness | Original worker; scoped display/brightness Confirm | Scoped-policy checks; source review/build | Preview, Cancel rollback, Confirm, reboot retained value |
| Volume | Original worker; codec current-session getter; preview suppressed persistence, Confirm saves audio/output_volume | Scoped-policy checks; source review/build | Preview/Cancel/Confirm, audible output, re-entry and reboot value; mute behavior |
| Timezone/RTC/SNTP | Original worker; correct system/tz key; offline RTC initialized, network SNTP preserved | Timezone scope check; source review/build | Change zone, reboot, correct clock; network time sync |
| AI General | Real worker; idle movement setting and explicit new boot opt-in | Source review/build | Off/Low/Medium/High actual frequency; Confirm/reboot; opt-in starts Stock, Stop/Home returns muted selector |
| AI power saving | Real worker; Stock runtime initializes/polls original power timer | Source review/build | Idle timeout; touch/speech wake; charging shutdown policy; Off setting |
| Servo calibration/test | Actual ZeroCalibrationWorker with powered hardware only after Start; scoped servo keys | Source review/build | Home/Set Home/Reset Default/Done; limits; saved calibration; VM_EN off after Stop/Home/reboot |
| Microphone test | Actual MicTestWorker, exclusive Native audio owner | Audio ownership/model checks; source review/build | Record/playback, retry, stop, switch to Companion/Stock with no lingering audio |
| RGB strip | Actual RgbTestWorker; IO init leaves motor power off | Source review/build | Colors/brightness/exit; motors remain unpowered |
| Account/status/reset | Actual AccountWorker, account write scope; original reset confirmation | Source review/build | Read status, back/retry. Unbind/factory reset is a separate explicit destructive device test; never part of automatic acceptance |
| Avatar service | Original AppAvatar, network/camera/video/messages/calls/motion | Source review/build | Bound account; messages, calls/video, remote motion; disconnect/retry; Stop ends radio/camera/audio/motor use |
| Stock AI | Original Application through Hal.startXiaozhi; reminder task/MCP/head/IMU/motion/speech/camera | Source review/build | Activation, wake word, listening/speaking, expression, head-pet/IMU, bounded MCP movement/LED/reminders; Stop Home; repeat startup |
| ESP-NOW | Original sender/receiver app in isolated radio runtime | Source review/build | Role/channel/peer IDs, filter, pose, laser, loss/retry; laser and motor off on exit |
| EzData | Original app, MQTT pairing/data controls | Source review/build | Pairing/code/account data, controls, failure/retry, motor-off exit |
| Dance | Original BLE avatar/motion/RGB app | Source review/build | Phone connects, motion/RGB commands, repeated sessions, stop/no stale BLE callbacks |
| App Center | Real installed-app launching and bounded vendor metadata browsing | Model/bounds/source checks; build | Each distinct app selected; catalog success/failure/cancel; return/switch without radio-driver collision |
| Firmware/version | Ten-tap original version worker; real update-version query | Source review/build | Correct build/version display and query result; error/retry/back |
| Tools/Game additions | IR learn/send, NFC URL keyboard/emulation, 2048 retained | Existing NEC/NFC/2048 and target-layout checks | IR timeout/send; NFC read/stop; keyboard navigation; 2048 gestures/back; switch into every restored service |
| Shared resource ownership | Mira/Stock/Native exclusivity; used radios restart only into stopped preview | Audio CAS/source checks; build | All ordering combinations; no duplicate Wi-Fi init, codec overlap or unexpected automatic restart |

## Explicit remaining differences

App Center is not a plugin installer: vendor entries represent replacement firmware. Current scope supports metadata and installed local apps, as previously accepted. Firmware update checks display available versions but do not install replacement firmware. Browser advanced settings POST remains disabled; ordinary Wi-Fi provisioning is restored. Acoustic/BLUFI provisioning is not enabled by this build. The original automatic welcome/servo wizard is replaced by manual setup/hardware entries to preserve stopped boot. These are not claimed as full stock parity.

No replacement image is flashed merely because it builds. Device acceptance requires approval/readiness for the exact reviewed image, followed by the checks above. Do not change active network settings or perform account/reset operations automatically.
