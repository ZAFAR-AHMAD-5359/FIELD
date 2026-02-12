// PluginEditor.h
// FIELD — Projection Engine v2.0
// Minimal 3-control UI with stereo visualization

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "StereoVisualization.h"

class FieldAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   private juce::Timer {
public:
    explicit FieldAudioProcessorEditor(FieldAudioProcessor&);
    ~FieldAudioProcessorEditor() override = default;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    // Reference to processor
    FieldAudioProcessor& audioProcessor;

    // UI Components
    juce::Slider energyKnob;
    juce::Slider fieldAmountKnob;
    juce::ComboBox modeSelector;

    juce::Label energyLabel;
    juce::Label fieldLabel;
    juce::Label modeLabel;
    juce::Label titleLabel;

    // Stereo visualization
    StereoVisualization stereoViz;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> energyAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fieldAmountAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment;

    // Colors
    const juce::Colour bgDark = juce::Colour(0xFF1A1A1A);
    const juce::Colour accentBlue = juce::Colour(0xFF4A90D9);
    const juce::Colour textLight = juce::Colour(0xFFE0E0E0);

    // Timer for visualization updates
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FieldAudioProcessorEditor)
};
