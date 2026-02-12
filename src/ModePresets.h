// ModePresets.h
// FIELD — Projection Engine
// Hardcoded mode configurations for Studio and Sound System

#pragma once

#include <array>

namespace ModePresets {

struct TapConfig {
    float delayMs;      // Delay time in milliseconds
    float pan;          // Pan position: -100 (full left) to +100 (full right)
    float lpCutoff;     // Low-pass filter cutoff frequency (Hz)
    float gainDb;       // Tap gain in dB
};

struct ModeConfig {
    std::array<TapConfig, 6> taps;
    float harmonicProfile;      // Harmonic character: 0.0-1.0 (Studio=lighter, SoundSystem=denser)
    float compensationTrim;     // Level matching trim in dB (calibrated)
    const char* name;
};

// STUDIO MODE
// Tighter delays, air-focused, no low-mid reinforcement
inline const ModeConfig STUDIO = {
    .taps = {{
        {6.0f,  -15.0f, 9500.0f, -12.0f},   // Tap 1
        {11.0f,  15.0f, 8500.0f, -13.5f},   // Tap 2
        {17.0f, -25.0f, 7200.0f, -15.0f},   // Tap 3
        {24.0f,  25.0f, 6000.0f, -17.0f},   // Tap 4
        {32.0f, -45.0f, 4800.0f, -19.0f},   // Tap 5
        {46.0f,  50.0f, 3800.0f, -22.0f}    // Tap 6
    }},
    .harmonicProfile = 0.5f,        // Lighter harmonic lift
    .compensationTrim = 0.0f,       // Reference (will be calibrated)
    .name = "Studio"
};

// SOUND SYSTEM MODE
// Wider delays, low-mid density increase when ENERGY > 40%
inline const ModeConfig SOUND_SYSTEM = {
    .taps = {{
        {8.0f,  -20.0f, 7000.0f, -11.0f},   // Tap 1
        {15.0f,  20.0f, 6200.0f, -12.5f},   // Tap 2
        {23.0f, -35.0f, 5400.0f, -14.5f},   // Tap 3
        {34.0f,  40.0f, 4400.0f, -16.5f},   // Tap 4
        {48.0f, -65.0f, 3400.0f, -18.5f},   // Tap 5
        {70.0f,  85.0f, 2600.0f, -21.0f}    // Tap 6
    }},
    .harmonicProfile = 0.8f,        // Denser low-mid body
    .compensationTrim = 0.0f,       // Will be calibrated for level matching
    .name = "Sound System"
};

// Helper to get mode by index
inline const ModeConfig& getMode(int index) {
    return (index == 0) ? STUDIO : SOUND_SYSTEM;
}

} // namespace ModePresets
