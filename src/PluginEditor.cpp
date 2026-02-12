// PluginEditor.cpp
// FIELD — Projection Engine v2.0

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FieldAudioProcessorEditor::FieldAudioProcessorEditor(FieldAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Window size: 600x400
    setSize(600, 400);

    // Title
    titleLabel.setText("FIELD — Projection Engine", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, textLight);
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // ENERGY Knob (Large, center)
    energyKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    energyKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    energyKnob.setColour(juce::Slider::rotarySliderFillColourId, accentBlue);
    energyKnob.setColour(juce::Slider::thumbColourId, accentBlue);
    addAndMakeVisible(energyKnob);

    energyLabel.setText("ENERGY", juce::dontSendNotification);
    energyLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    energyLabel.setColour(juce::Label::textColourId, textLight);
    energyLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(energyLabel);

    // FIELD AMOUNT Knob (Smaller, left)
    fieldAmountKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    fieldAmountKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    fieldAmountKnob.setColour(juce::Slider::rotarySliderFillColourId, accentBlue);
    addAndMakeVisible(fieldAmountKnob);

    fieldLabel.setText("FIELD", juce::dontSendNotification);
    fieldLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    fieldLabel.setColour(juce::Label::textColourId, textLight);
    fieldLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(fieldLabel);

    // MODE Selector (Right)
    modeSelector.addItem("Studio", 1);
    modeSelector.addItem("Sound System", 2);
    modeSelector.setSelectedId(1);
    addAndMakeVisible(modeSelector);

    modeLabel.setText("MODE", juce::dontSendNotification);
    modeLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    modeLabel.setColour(juce::Label::textColourId, textLight);
    modeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(modeLabel);

    // Attachments
    energyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "energy", energyKnob);

    fieldAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "field_amount", fieldAmountKnob);

    modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.apvts, "mode", modeSelector);

    // Stereo visualization
    addAndMakeVisible(stereoViz);

    // Start timer for visualization updates (30 Hz)
    startTimerHz(30);
}

//==============================================================================
void FieldAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background gradient
    g.fillAll(bgDark);

    juce::ColourGradient gradient(
        bgDark.brighter(0.1f), 0.0f, 0.0f,
        bgDark.darker(0.2f), 0.0f, static_cast<float>(getHeight()),
        false
    );
    g.setGradientFill(gradient);
    g.fillAll();

    // Version label
    g.setColour(textLight.withAlpha(0.5f));
    g.setFont(juce::Font(10.0f));
    g.drawText("v2.0", getWidth() - 50, 10, 40, 20, juce::Justification::centred);
}

void FieldAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // Header (40px)
    auto header = bounds.removeFromTop(40);
    titleLabel.setBounds(header.reduced(10));

    // Main area
    auto mainArea = bounds.reduced(20);

    // Center area for large ENERGY knob
    auto centerArea = mainArea.withSizeKeepingCentre(180, 220);
    energyLabel.setBounds(centerArea.removeFromTop(20));
    energyKnob.setBounds(centerArea.removeFromTop(180));

    // Left area for FIELD knob
    auto leftArea = juce::Rectangle<int>(60, 120, 100, 140);
    leftArea = leftArea.withCentre(juce::Point<int>(120, getHeight() / 2 + 20));
    fieldLabel.setBounds(leftArea.removeFromTop(20));
    fieldAmountKnob.setBounds(leftArea.removeFromTop(100));

    // Right area for MODE selector
    auto rightArea = juce::Rectangle<int>(getWidth() - 180, 120, 120, 100);
    rightArea = rightArea.withCentre(juce::Point<int>(getWidth() - 120, getHeight() / 2 + 20));
    modeLabel.setBounds(rightArea.removeFromTop(20));
    modeSelector.setBounds(rightArea.removeFromTop(30).reduced(10, 0));

    // Bottom area for stereo visualization
    auto vizArea = getLocalBounds().removeFromBottom(80).reduced(40, 10);
    stereoViz.setBounds(vizArea);
}

void FieldAudioProcessorEditor::timerCallback()
{
    // Update stereo visualization with current audio levels
    auto levels = audioProcessor.getCurrentLevels();
    stereoViz.update(levels.left, levels.right);
}
