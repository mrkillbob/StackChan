# Source and modifications

This integration preserves341stable catalog IDs and adapts selected routines and the drawGear helper from Coreymillia/341-Xscreensaver-Esp32-24232028-, commit337adf7f983b20a889e6c839696584142451a6e8.

Original source SHA-256:9aa4dfc0a71ca21ddbaf1c2ef83341c5d8bd20cb0ec5ad8c42d88cfb7489486e8.

Copyright (c)2024 ESP32 Screensaver Universe Project. Complete upstream MIT license and additional XScreensaver attribution retained in LICENSE.upstream. Source snapshot retained in upstream-source.txt. No M5Core-specific wrapper copied.

Local modifications: portable RGB565 clipped raster drawing; Arduino drawing/random helper replacement; remove blocking delays; lift function-local static state into per-effect namespace and reset it explicitly (including initialized arrays); saturate byte-color conversions and Lyapunov intensity before floating-point conversion. Initialize switch-assigned color locals to BLACK for a defined fallback. Original effects remain separately named functions. Text uses an original compact3x5 uppercase/digit font; rounded rectangles currently use square corners.

Private source candidate only. The upstream notice refers to original XScreensaver components with MIT/GPL licenses. A component-by-component license/provenance mapping remains required before redistribution; this file does not certify that mapping.

Current selection policy removes 126 entire implementations: the prior 125 light/flashing effects and red/blue full backgrounds (including ChessModels), plus Moire (saved slide ID 226 / catalog index 225) after a reported device freeze. Their stable IDs remain unavailable tombstones; 215 catalog routines plus DVD remain. Moire2 (saved slide ID 225) is a distinct effect and remains available. Intentional neutral dark gray is retained. The immutable upstream reference snapshot is provenance only and is not compiled. The generator reads the same exclusion policy and cannot regenerate removed implementations.

The validation.json file records the original integration benchmark; it does not describe the current filtered catalog or certify device timing.
