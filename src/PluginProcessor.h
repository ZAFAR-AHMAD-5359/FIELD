// PluginProcessor.h
// FIELD — Projection Engine v2.0
// Main audio processor with 3-parameter simplified interface

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "TapProcessor.h"
#include "HarmonicGenerator.h"
#include "SoftCeiling.h"
#include "ModePresets.h"

class FieldAudioProcessor : public juce::AudioProcessor {
public:
    FieldAudioProcessor();
    ~FieldAudioProcessor() override = default;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.1; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Public access to APVTS for UI
    juce::AudioProcessorValueTreeState apvts;

    // Audio levels for visualization
    struct AudioLevels {
        float left = 0.0f;
        float right = 0.0f;
    };

    AudioLevels getCurrentLevels() const {
        return currentLevels.load();
    }

private:
    //==============================================================================
    // DSP Components
    std::array<TapProcessor, 6> tapProcessors;  // Fixed 6 taps
    HarmonicGenerator harmonicGen;
    SoftCeiling softCeiling;

    // Parameter caching (lock-free audio thread access)
    std::atomic<float>* modeParam = nullptr;
    std::atomic<float>* energyParam = nullptr;
    std::atomic<float>* fieldAmountParam = nullptr;

    // Smoothing
    juce::SmoothedValue<float> dryWetSmoothed;

    // Current mode index (0 = Studio, 1 = Sound System)
    int currentModeIndex = 0;

    // Audio levels for visualization (thread-safe)
    std::atomic<AudioLevels> currentLevels;

    //==============================================================================
    // Parameter layout
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Update taps from mode preset
    void updateTapsFromMode(const ModePresets::ModeConfig& mode);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FieldAudioProcessor)
};
