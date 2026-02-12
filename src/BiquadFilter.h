#pragma once

/**
 * Second-order biquad IIR filter supporting LP, HP, and BP modes.
 */
class BiquadFilter
{
public:
    enum class Type
    {
        LowPass = 0,
        HighPass = 1,
        BandPass = 2
    };

    BiquadFilter();

    void prepare(double sampleRate);
    void reset();

    void setType(Type newType);
    void setFrequency(float freqHz);
    void setQ(float newQ);

    Type getType() const { return filterType; }
    float getFrequency() const { return frequency; }

    float process(float inputSample);

private:
    void recalculateCoefficients();

    Type filterType = Type::LowPass;
    float frequency = 6000.0f;
    float q = 0.707f; // Butterworth Q
    double sampleRate = 44100.0;

    // Biquad coefficients
    float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
    float a1 = 0.0f, a2 = 0.0f;

    // State variables (Direct Form II Transposed)
    float z1 = 0.0f, z2 = 0.0f;

    bool needsRecalc = true;
};
