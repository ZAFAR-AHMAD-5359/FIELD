#include "BiquadFilter.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

BiquadFilter::BiquadFilter() = default;

void BiquadFilter::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    needsRecalc = true;
    reset();
}

void BiquadFilter::reset()
{
    z1 = 0.0f;
    z2 = 0.0f;
}

void BiquadFilter::setType(Type newType)
{
    if (filterType != newType)
    {
        filterType = newType;
        needsRecalc = true;
    }
}

void BiquadFilter::setFrequency(float freqHz)
{
    freqHz = std::clamp(freqHz, 20.0f, 20000.0f);
    if (frequency != freqHz)
    {
        frequency = freqHz;
        needsRecalc = true;
    }
}

void BiquadFilter::setQ(float newQ)
{
    newQ = std::clamp(newQ, 0.1f, 10.0f);
    if (q != newQ)
    {
        q = newQ;
        needsRecalc = true;
    }
}

float BiquadFilter::process(float inputSample)
{
    if (needsRecalc)
    {
        recalculateCoefficients();
        needsRecalc = false;
    }

    // Direct Form II Transposed
    float output = b0 * inputSample + z1;
    z1 = b1 * inputSample - a1 * output + z2;
    z2 = b2 * inputSample - a2 * output;

    return output;
}

void BiquadFilter::recalculateCoefficients()
{
    float w0 = static_cast<float>(2.0 * M_PI * frequency / sampleRate);
    float cosW0 = std::cos(w0);
    float sinW0 = std::sin(w0);
    float alpha = sinW0 / (2.0f * q);

    float a0;

    switch (filterType)
    {
        case Type::LowPass:
        {
            b0 = (1.0f - cosW0) / 2.0f;
            b1 = 1.0f - cosW0;
            b2 = (1.0f - cosW0) / 2.0f;
            a0 = 1.0f + alpha;
            a1 = -2.0f * cosW0;
            a2 = 1.0f - alpha;
            break;
        }
        case Type::HighPass:
        {
            b0 = (1.0f + cosW0) / 2.0f;
            b1 = -(1.0f + cosW0);
            b2 = (1.0f + cosW0) / 2.0f;
            a0 = 1.0f + alpha;
            a1 = -2.0f * cosW0;
            a2 = 1.0f - alpha;
            break;
        }
        case Type::BandPass:
        {
            b0 = alpha;
            b1 = 0.0f;
            b2 = -alpha;
            a0 = 1.0f + alpha;
            a1 = -2.0f * cosW0;
            a2 = 1.0f - alpha;
            break;
        }
    }

    // Normalize by a0
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
}
