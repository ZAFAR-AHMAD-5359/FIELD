#pragma once
#include <vector>
#include <cstddef>

/**
 * Circular buffer delay line with linear interpolation.
 * Max delay: 100ms at any sample rate.
 */
class DelayLine
{
public:
    DelayLine();

    void prepare(double sampleRate, int maxDelayMs = 100);
    void reset();

    void setDelayMs(float delayMs);
    float getDelayMs() const { return currentDelayMs; }

    float process(float inputSample);

private:
    std::vector<float> buffer;
    size_t writeIndex = 0;
    size_t bufferSize = 0;
    float currentDelayMs = 0.0f;
    float delaySamples = 0.0f;
    double sampleRate = 44100.0;
};
