#include "DelayLine.h"
#include <algorithm>
#include <cmath>

DelayLine::DelayLine() = default;

void DelayLine::prepare(double newSampleRate, int maxDelayMs)
{
    sampleRate = newSampleRate;
    // Buffer size = max delay in samples + 1 for interpolation
    bufferSize = static_cast<size_t>(std::ceil(sampleRate * maxDelayMs / 1000.0)) + 2;
    buffer.resize(bufferSize, 0.0f);
    reset();
}

void DelayLine::reset()
{
    std::fill(buffer.begin(), buffer.end(), 0.0f);
    writeIndex = 0;
}

void DelayLine::setDelayMs(float delayMs)
{
    currentDelayMs = std::clamp(delayMs, 0.0f, 100.0f);
    delaySamples = static_cast<float>(currentDelayMs * sampleRate / 1000.0);
}

float DelayLine::process(float inputSample)
{
    // Write input to buffer
    buffer[writeIndex] = inputSample;

    // Calculate read position with linear interpolation
    float readPos = static_cast<float>(writeIndex) - delaySamples;
    if (readPos < 0.0f)
        readPos += static_cast<float>(bufferSize);

    // Integer and fractional parts for interpolation
    auto readIndex0 = static_cast<size_t>(readPos);
    float frac = readPos - static_cast<float>(readIndex0);
    size_t readIndex1 = (readIndex0 + 1) % bufferSize;
    readIndex0 = readIndex0 % bufferSize;

    // Linear interpolation
    float output = buffer[readIndex0] + frac * (buffer[readIndex1] - buffer[readIndex0]);

    // Advance write pointer
    writeIndex = (writeIndex + 1) % bufferSize;

    return output;
}
