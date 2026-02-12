# FIELD — Projection Engine

**v2.0.0** - Consumer projection plugin with minimal controls

---

## Quick Build

### macOS (Universal Binary - arm64 + x86_64)

```bash
cmake -B build -G Xcode \
  -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0

cmake --build build --config Release
```

### Windows (VST3)

```bash
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

---

## Features

- **3 Controls**: MODE (Studio/Sound System), ENERGY, FIELD AMOUNT
- **Harmonic Exciter**: Even-dominant harmonics for structural lift
- **6 Hardcoded Taps**: Optimized presets per mode
- **Level-Matched Modes**: Instant switching without loudness jumps
- **Minimal UI**: Clean, commercial design

---

## Architecture

**Signal Flow:**
```
Input → Pre-Atten (-6dB) → Harmonic Gen → Soft Ceiling →
6-Tap Field → Mode Compensation → Dry/Wet → Output
```

**DSP Modules:**
- `HarmonicGenerator`: Even-dominant exciter
- `SoftCeiling`: Transparent limiter at -0.5 dBFS
- `TapProcessor`: Simplified delay → pan → filter → gain
- `ModePresets`: Hardcoded Studio and Sound System configs

---

## Requirements

- JUCE 8.0.0 (fetched automatically)
- CMake 3.22+
- macOS 11.0+ or Windows 10+
- Xcode 13+ (macOS) or Visual Studio 2022 (Windows)

---

## Platform Support

| Platform | Formats |
|----------|---------|
| macOS    | VST3, AU |
| Windows  | VST3 |

---

Built for Ragein by Zafar Ahmad
