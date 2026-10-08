# Custom boot verification

The actual companion Settings variant is tested against NVS API stubs: even a requested read/write namespace opens read-only, and no setter, erase or commit reaches NVS. The handshake parser tests an RFC acceptance example plus invalid/missing/duplicate headers and unsupported extensions. These tests do not execute firmware or physical IO.

From the fork root, compile the two test programs with the existing C++17 compiler. On this Mac use the explicit Xcode SDK libc++ include path if its default header search is unavailable:

```sh
xcrun clang++ -std=c++17 -isystem /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1 -I firmware/main/companion companion/host/boot-tests/test_handshake.cpp -o /tmp/stackchan-handshake-test
/tmp/stackchan-handshake-test
xcrun clang++ -std=c++17 -isystem /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1 -I companion/host/boot-tests -I firmware/xiaozhi-esp32/main companion/host/boot-tests/test_readonly.cpp firmware/main/companion/readonly_settings.cc -o /tmp/stackchan-readonly-test
/tmp/stackchan-readonly-test
```

Firmware: source the existing pinned ESP-IDF 5.5.4 export script, then run `cmake --build build --target lang_header --parallel 1` and `cmake --build build --parallel 1` in `firmware`. `STACKCHAN_COMPANION_BOOT` defaults ON and selects companion-owned read-only variants without editing fetched upstream sources. The retained `boot_compile.py` records the one-worker network-denied invocation and owned-process cleanup; adjust its receipt output location deliberately before reuse. No flash is part of these commands.

The first bounded recompilation timed out, then the cached remainder passed, followed by a final incremental build for the exact final status source. All receipts are retained. `COREAUDIO-METADATA.json` is read-only: endpoints present at 48 kHz and 512 frames, no IO or routing acceptance. The final image has not been flashed.
