#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RotaryKnob.h"
#include "LookAndFeel.h"
#include "Parameters.h"
#include "LevelMeter.h"
#include "Gui/PresetPanel.h"

class DelayAudioProcessorEditor  : public juce::AudioProcessorEditor,
    private juce::AudioProcessorParameter::Listener
{
public:
    DelayAudioProcessorEditor (DelayAudioProcessor&);
    ~DelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void parameterValueChanged(int, float) override;
    void parameterGestureChanged(int, bool) override {}

    void updateDelayKnobs(bool tempoSyncActive);

    DelayAudioProcessor& audioProcessor;

    RotaryKnob gainKnob{ "Output Gain", audioProcessor.apvts, gainParamID ,true };
    RotaryKnob mixKnob{ "Mix", audioProcessor.apvts, mixParamID };
    RotaryKnob delayTimeKnob{ "Delay Time", audioProcessor.apvts, delayTimeParamID };
    RotaryKnob feedbackKnob{ "Feedback", audioProcessor.apvts, feedbackParamID, true };
    RotaryKnob stereoKnob{ "Stereo", audioProcessor.apvts, stereoParamID, true };
    RotaryKnob lowCutKnob{ "Low Cut", audioProcessor.apvts, lowCutParamID };
    RotaryKnob highCutKnob{ "High Cut", audioProcessor.apvts, highCutParamID };
    RotaryKnob qFactorKnob{ "Q Factor", audioProcessor.apvts, qFactorParamID };
    RotaryKnob driveKnob{ "Drive", audioProcessor.apvts, driveParamID };
    RotaryKnob delayNoteKnob{ "Note", audioProcessor.apvts, delayNoteParamID };
    juce::TextButton tempoSyncButton;

    LevelMeter meter;

    juce::AudioProcessorValueTreeState::ButtonAttachment tempoSyncAttachment
    {
        audioProcessor.apvts, tempoSyncParamID.getParamID(), tempoSyncButton
    };

    juce::GroupComponent delayGroup, feedbackGroup, outputGroup;

    juce::ImageButton bypassButton;
    juce::AudioProcessorValueTreeState::ButtonAttachment bypassAttachment
    {
        audioProcessor.apvts, bypassParamID.getParamID(), bypassButton
    };

    MainLookAndFeel mainLF;

    Gui::PresetPanel presetPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
