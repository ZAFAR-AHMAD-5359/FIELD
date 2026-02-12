// PluginProcessor.cpp
// FIELD — Projection Engine v2.0

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FieldAudioProcessor::FieldAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Cache parameter pointers for lock-free audio thread access
    modeParam = apvts.getRawParameterValue("mode");
    energyParam = apvts.getRawParameterValue("energy");
    fieldAmountParam = apvts.getRawParameterValue("field_amount");
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout FieldAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // MODE: Studio (0) or Sound System (1)
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "mode",
        "Mode",
        juce::StringArray{"Studio", "Sound System"},
        0));

    // ENERGY: Harmonic excitation intensity (0-100)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "energy",
        "Energy",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 1) + "%"; }));

    // FIELD AMOUNT: Wet/dry blend (0-100)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "field_amount",
        "Field Amount",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String(value, 1) + "%"; }));

    return {params.begin(), params.end()};
}

//==============================================================================
void FieldAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare all tap processors
    for (auto& tap : tapProcessors) {
        tap.prepare(sampleRate);
    }

    // Setup smoothing for dry/wet (20ms ramp time)
    dryWetSmoothed.reset(sampleRate, 0.02);
    dryWetSmoothed.setCurrentAndTargetValue(0.5f);

    // Initialize with Studio mode
    updateTapsFromMode(ModePresets::STUDIO);
    currentModeIndex = 0;
}

void FieldAudioProcessor::releaseResources()
{
    for (auto& tap : tapProcessors) {
        tap.reset();
    }
}

bool FieldAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Stereo in, stereo out
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo();
}

//==============================================================================
void FieldAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();

    // Get parameter values
    int modeIndex = static_cast<int>(modeParam->load());
    float energy = energyParam->load();
    float fieldAmount = fieldAmountParam->load() / 100.0f;  // Convert to 0-1

    // Check for mode change
    if (modeIndex != currentModeIndex) {
        currentModeIndex = modeIndex;
        const auto& mode = ModePresets::getMode(modeIndex);
        updateTapsFromMode(mode);
    }

    // Get current mode
    const auto& mode = ModePresets::getMode(modeIndex);

    // Update harmonic generator
    harmonicGen.setEnergy(energy);
    harmonicGen.setHarmonicProfile(mode.harmonicProfile);

    // Update dry/wet smoothing target
    dryWetSmoothed.setTargetValue(fieldAmount);

    // Mode compensation trim
    float compensationGain = juce::Decibels::decibelsToGain(mode.compensationTrim);

    // Store dry signal
    auto dryL = buffer.getSample(0, 0);
    auto dryR = buffer.getSample(1, 0);

    // Process each sample
    for (int sample = 0; sample < numSamples; ++sample) {
        float L = buffer.getSample(0, sample);
        float R = buffer.getSample(1, sample);

        // Store dry for later mix
        float drySignalL = L;
        float drySignalR = R;

        // 1. Mono sum
        float mono = (L + R) * 0.5f;

        // 2. Pre-attenuation (-6 dB)
        mono *= 0.5f;

        // 3. Harmonic generator
        float excited = harmonicGen.processSample(mono);

        // 4. Soft ceiling limiter
        excited = softCeiling.processSample(excited);

        // 5. 6-tap early field
        float wetL = 0.0f;
        float wetR = 0.0f;

        for (auto& tap : tapProcessors) {
            auto stereo = tap.process(excited);
            wetL += stereo.left;
            wetR += stereo.right;
        }

        // 6. Mode compensation trim
        wetL *= compensationGain;
        wetR *= compensationGain;

        // 7. Dry/wet mix (smoothed)
        float wetAmount = dryWetSmoothed.getNextValue();
        float dryAmount = 1.0f - wetAmount;

        buffer.setSample(0, sample, drySignalL * dryAmount + wetL * wetAmount);
        buffer.setSample(1, sample, drySignalR * dryAmount + wetR * wetAmount);
    }

    // Calculate RMS levels for visualization
    float sumL = 0.0f, sumR = 0.0f;
    for (int sample = 0; sample < numSamples; ++sample) {
        float L = buffer.getSample(0, sample);
        float R = buffer.getSample(1, sample);
        sumL += L * L;
        sumR += R * R;
    }

    AudioLevels levels;
    levels.left = std::sqrt(sumL / numSamples);
    levels.right = std::sqrt(sumR / numSamples);
    currentLevels.store(levels);
}

//==============================================================================
void FieldAudioProcessor::updateTapsFromMode(const ModePresets::ModeConfig& mode)
{
    for (int i = 0; i < 6; ++i) {
        const auto& tapConfig = mode.taps[i];
        tapProcessors[i].setParameters(
            tapConfig.delayMs,
            tapConfig.pan,
            tapConfig.lpCutoff,
            tapConfig.gainDb
        );
    }
}

//==============================================================================
juce::AudioProcessorEditor* FieldAudioProcessor::createEditor()
{
    return new FieldAudioProcessorEditor(*this);
}

//==============================================================================
void FieldAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void FieldAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FieldAudioProcessor();
}
