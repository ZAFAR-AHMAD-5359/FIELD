// HarmonicGenerator.h
// FIELD — Projection Engine
// Even-dominant harmonic exciter with mode-specific profiles

#pragma once

#include <cmath>

class HarmonicGenerator {
public:
    HarmonicGenerator() = default;

    // Set harmonic intensity (0-100)
    void setEnergy(float energyPercent) {
        energy = juce::jlimit(0.0f, 100.0f, energyPercent) / 100.0f;
        updateCoefficients();
    }

    // Set harmonic profile (0.0-1.0, lighter to denser)
    void setHarmonicProfile(float profile) {
        harmonicProfile = juce::jlimit(0.0f, 1.0f, profile);
        updateCoefficients();
    }

    // Process single sample
    float processSample(float input) {
        if (energy < 0.001f) {
            // Phase-neutral bypass when ENERGY = 0
            return input;
        }

        // Soft-knee waveshaper with even-dominant harmonics
        // y = x + a*x^2 + b*tanh(x) + c*x^3

        float x = input;

        // Even harmonics (x^2) - dominant
        float evenHarm = evenCoeff * x * x * juce::dsp::FastMathApproximations::tanh(x);

        // Odd harmonics (x^3, tanh) - very light presence
        float oddHarm = oddCoeff * (x * x * x * 0.1f + juce::dsp::FastMathApproximations::tanh(x * 1.5f) * 0.05f);

        // Combine
        float output = x + evenHarm + oddHarm;

        // Soft clip to prevent harsh peaks
        output = juce::dsp::FastMathApproximations::tanh(output * 0.9f);

        return output;
    }

private:
    float energy = 0.0f;            // 0-1 range
    float harmonicProfile = 0.5f;   // 0-1 range (lighter to denser)

    float evenCoeff = 0.0f;         // Even harmonic coefficient
    float oddCoeff = 0.0f;          // Odd harmonic coefficient

    void updateCoefficients() {
        // Logarithmic scaling for more natural feel
        float energyScaled = std::pow(energy, 1.5f);

        // Even harmonics: dominant, increases with energy
        // Profile affects intensity (Studio=lighter, SoundSystem=denser)
        evenCoeff = energyScaled * (0.2f + harmonicProfile * 0.3f);

        // Odd harmonics: very light, only at higher energy levels
        // Threshold: only appears above 40% energy for Sound System mode
        float oddThreshold = (harmonicProfile > 0.6f && energy > 0.4f) ? energy - 0.4f : 0.0f;
        oddCoeff = oddThreshold * 0.15f * harmonicProfile;

        // Cap to prevent harshness
        evenCoeff = juce::jlimit(0.0f, 0.5f, evenCoeff);
        oddCoeff = juce::jlimit(0.0f, 0.12f, oddCoeff);
    }
};
