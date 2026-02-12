// StereoVisualization.h
// FIELD — Projection Engine
// Professional stereo field visualization

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class StereoVisualization : public juce::Component,
                             private juce::Timer {
public:
    StereoVisualization() {
        // Start update timer (30 Hz for smooth animation)
        startTimerHz(30);
    }

    ~StereoVisualization() override {
        stopTimer();
    }

    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();

        // Background
        g.setColour(juce::Colour(0xFF0A0A0A));
        g.fillRoundedRectangle(bounds, 8.0f);

        // Border
        g.setColour(accentBlue.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds.reduced(1.0f), 8.0f, 1.5f);

        // Center line
        float centerX = bounds.getCentreX();
        g.setColour(juce::Colour(0xFF333333));
        g.drawLine(centerX, bounds.getY() + 10, centerX, bounds.getBottom() - 10, 1.0f);

        // L/R Labels
        g.setColour(textLight.withAlpha(0.6f));
        g.setFont(juce::Font(juce::FontOptions(10.0f).withStyle("Bold")));
        g.drawText("L", bounds.getX() + 10, bounds.getCentreY() - 5, 20, 10, juce::Justification::left);
        g.drawText("R", bounds.getRight() - 30, bounds.getCentreY() - 5, 20, 10, juce::Justification::right);

        // Stereo width indicator
        drawStereoWidth(g, bounds);

        // Peak meters
        drawPeakMeters(g, bounds);
    }

    void resized() override {}

    // Update audio levels
    void update(float leftLevel, float rightLevel) {
        // RMS smoothing
        const float smoothing = 0.3f;
        smoothedLeft = smoothedLeft * (1.0f - smoothing) + leftLevel * smoothing;
        smoothedRight = smoothedRight * (1.0f - smoothing) + rightLevel * smoothing;

        // Peak detection with decay
        if (leftLevel > peakLeft) {
            peakLeft = leftLevel;
        } else {
            peakLeft *= peakDecay;
        }

        if (rightLevel > peakRight) {
            peakRight = rightLevel;
        } else {
            peakRight *= peakDecay;
        }

        // Calculate stereo width (0 = mono, 1 = full stereo)
        float sum = smoothedLeft + smoothedRight;
        float diff = std::abs(smoothedLeft - smoothedRight);
        stereoWidth = (sum > 0.001f) ? (diff / sum) : 0.0f;
    }

private:
    // Audio levels
    float smoothedLeft = 0.0f;
    float smoothedRight = 0.0f;
    float peakLeft = 0.0f;
    float peakRight = 0.0f;
    float stereoWidth = 0.0f;

    // Constants
    const float peakDecay = 0.95f;
    const juce::Colour accentBlue = juce::Colour(0xFF4A90D9);
    const juce::Colour textLight = juce::Colour(0xFFE0E0E0);

    void timerCallback() override {
        repaint();
    }

    void drawStereoWidth(juce::Graphics& g, juce::Rectangle<float> bounds) {
        auto widthArea = bounds.reduced(20, 15);
        float centerX = widthArea.getCentreX();
        float y = widthArea.getCentreY();

        // Stereo width arc
        float arcWidth = widthArea.getWidth() * 0.7f;
        float widthAmount = stereoWidth * 0.5f;  // 0 to 0.5

        // Draw stereo spread
        juce::Path leftArc;
        leftArc.addCentredArc(
            centerX - arcWidth * widthAmount, y,
            15.0f, 15.0f,
            0.0f, 0.0f, juce::MathConstants<float>::pi,
            true
        );

        juce::Path rightArc;
        rightArc.addCentredArc(
            centerX + arcWidth * widthAmount, y,
            15.0f, 15.0f,
            0.0f, juce::MathConstants<float>::pi, juce::MathConstants<float>::twoPi,
            true
        );

        // Gradient fill for stereo bloom
        juce::ColourGradient gradient(
            accentBlue.withAlpha(0.8f), centerX - arcWidth * widthAmount, y,
            accentBlue.withAlpha(0.0f), centerX, y,
            false
        );
        g.setGradientFill(gradient);
        g.fillPath(leftArc);

        juce::ColourGradient gradient2(
            accentBlue.withAlpha(0.8f), centerX + arcWidth * widthAmount, y,
            accentBlue.withAlpha(0.0f), centerX, y,
            false
        );
        g.setGradientFill(gradient2);
        g.fillPath(rightArc);

        // Center dot
        g.setColour(accentBlue);
        g.fillEllipse(centerX - 3, y - 3, 6, 6);
    }

    void drawPeakMeters(juce::Graphics& g, juce::Rectangle<float> bounds) {
        auto meterArea = bounds.reduced(15, bounds.getHeight() * 0.3f);
        float meterHeight = 4.0f;
        float leftY = meterArea.getY();
        float rightY = meterArea.getBottom() - meterHeight;

        // Left meter
        float leftWidth = meterArea.getWidth() * 0.5f * smoothedLeft;
        g.setColour(accentBlue.withAlpha(0.6f));
        g.fillRoundedRectangle(
            meterArea.getCentreX() - leftWidth, leftY,
            leftWidth, meterHeight,
            2.0f
        );

        // Left peak indicator
        if (peakLeft > 0.01f) {
            float peakX = meterArea.getCentreX() - (meterArea.getWidth() * 0.5f * peakLeft);
            g.setColour(accentBlue.brighter(0.5f));
            g.fillRoundedRectangle(peakX - 1, leftY - 1, 2, meterHeight + 2, 1.0f);
        }

        // Right meter
        float rightWidth = meterArea.getWidth() * 0.5f * smoothedRight;
        g.setColour(accentBlue.withAlpha(0.6f));
        g.fillRoundedRectangle(
            meterArea.getCentreX(), rightY,
            rightWidth, meterHeight,
            2.0f
        );

        // Right peak indicator
        if (peakRight > 0.01f) {
            float peakX = meterArea.getCentreX() + (meterArea.getWidth() * 0.5f * peakRight);
            g.setColour(accentBlue.brighter(0.5f));
            g.fillRoundedRectangle(peakX - 1, rightY - 1, 2, meterHeight + 2, 1.0f);
        }
    }
};
