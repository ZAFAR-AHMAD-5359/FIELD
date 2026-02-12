// SoftCeiling.h
// FIELD — Projection Engine
// Transparent soft-knee limiter at -0.5 dBFS

#pragma once

#include <cmath>

class SoftCeiling {
public:
    SoftCeiling() = default;

    // Process single sample
    float processSample(float input) {
        float absInput = std::abs(input);

        if (absInput < threshold) {
            // Below threshold: pass through unchanged
            return input;
        } else if (absInput < kneeEnd) {
            // In soft knee region: gentle compression
            float overshoot = absInput - threshold;
            float kneeFactor = overshoot / kneeWidth;
            float gainReduction = 1.0f - (kneeFactor * kneeFactor * 0.5f);

            return input * gainReduction;
        } else {
            // Above knee: hard limit with soft transition
            float sign = (input >= 0.0f) ? 1.0f : -1.0f;
            return sign * (threshold + (kneeWidth * 0.5f));
        }
    }

private:
    // Threshold: -0.5 dBFS = 0.891 linear
    static constexpr float threshold = 0.891f;

    // Soft knee width: 3 dB range for smooth transition
    static constexpr float kneeWidth = 0.15f;
    static constexpr float kneeEnd = threshold + kneeWidth;
};
