// TapProcessor.h
// FIELD — Projection Engine
// Simplified tap processor: Delay → Pan → Filter → Gain (no drift/modulation)

#pragma once

#include "DelayLine.h"
#include "BiquadFilter.h"

/**
 * Single tap processor for FIELD
 * Signal chain: Delay → Pan → Filter → Gain
 * Processes mono input, outputs stereo (L/R after panning)
 */
class TapProcessor {
public:
    TapProcessor() = default;

    void prepare(double sampleRate) {
        delayLine.prepare(sampleRate);
        filter.setSampleRate(sampleRate);
    }

    void reset() {
        delayLine.reset();
        filter.reset();
    }

    // Set all parameters at once from ModePresets::TapConfig
    void setParameters(float delayMs, float pan, float lpCutoff, float gainDb) {
        setDelayMs(delayMs);
        setPan(pan);
        setFilterFrequency(lpCutoff);
        setGainDb(gainDb);
    }

    // Individual parameter setters
    void setDelayMs(float delayMs) {
        delayLine.setDelayMs(delayMs);
    }

    void setPan(float pan) {
        // Pan: -100 (full L) to +100 (full R)
        panValue = juce::jlimit(-100.0f, 100.0f, pan);
        updatePanGains();
    }

    void setFilterFrequency(float freqHz) {
        // Always use low-pass filter for FIELD
        filter.setParameters(BiquadFilter::Type::LowPass, freqHz, 0.707f);
    }

    void setGainDb(float gainDb) {
        this->gainDb = gainDb;
        updateGainLinear();
    }

    // Process mono input, returns stereo pair
    struct StereoSample {
        float left;
        float right;
    };

    StereoSample process(float monoInput) {
        // 1. Delay
        float delayed = delayLine.process(monoInput);

        // 2. Filter
        float filtered = filter.process(delayed);

        // 3. Gain (with smoothing)
        gainLinear += (targetGainLinear - gainLinear) * smoothingCoeff;
        float gained = filtered * gainLinear;

        // 4. Pan (with smoothing)
        panGainL += (targetPanGainL - panGainL) * smoothingCoeff;
        panGainR += (targetPanGainR - panGainR) * smoothingCoeff;

        return {
            gained * panGainL,  // Left
            gained * panGainR   // Right
        };
    }

private:
    DelayLine delayLine;
    BiquadFilter filter;

    // Panning
    float panValue = 0.0f;       // -100 to +100
    float panGainL = 0.707f;     // cos(pi/4) - center
    float panGainR = 0.707f;     // sin(pi/4) - center
    float targetPanGainL = 0.707f;
    float targetPanGainR = 0.707f;

    // Gain
    float gainDb = -12.0f;
    float gainLinear = 0.25f;
    float targetGainLinear = 0.25f;

    static constexpr float smoothingCoeff = 0.001f;
    static constexpr float pi = 3.14159265359f;

    void updatePanGains() {
        // Constant-power panning
        // Pan range: -100 to +100 -> angle: 0 to pi/2
        float normalizedPan = (panValue + 100.0f) / 200.0f;  // 0 to 1
        float angle = normalizedPan * pi * 0.5f;             // 0 to pi/2

        targetPanGainL = std::cos(angle);
        targetPanGainR = std::sin(angle);
    }

    void updateGainLinear() {
        targetGainLinear = juce::Decibels::decibelsToGain(gainDb);
    }
};
