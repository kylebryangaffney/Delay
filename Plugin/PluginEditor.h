#pragma once

#include <JuceHeader.h>

#include "../Plugin/PluginProcessor.h"
#include "../Gui/RotaryKnob.h"
#include "../Gui/AsymmetricalRotaryKnob.h"
#include "../Gui/LookAndFeel.h"
#include "../Gui/LevelMeter.h"
#include "../Gui/PresetPanel.h"
#include "../Service/Parameters.h"

class DelayAudioProcessorEditor : public juce::AudioProcessorEditor,
    private juce::AudioProcessorParameter::Listener
{
public:
    DelayAudioProcessorEditor(DelayAudioProcessor&);
    ~DelayAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    void parameterValueChanged(int, float) override;
    void parameterGestureChanged(int, bool) override {}

    void updateDelayKnobs(bool tempoSyncActive);

    //==============================================================================
    DelayAudioProcessor& audioProcessor;

    // Knobs
    AsymmetricalRotaryKnob gainKnob{ "Output Gain", audioProcessor.apvts, gainParamID };
    RotaryKnob mixKnob{ "Mix",         audioProcessor.apvts, mixParamID };
    RotaryKnob delayTimeKnob{ "Delay Time",  audioProcessor.apvts, delayTimeParamID };
    RotaryKnob feedbackKnob{ "Feedback",    audioProcessor.apvts, feedbackParamID };
    RotaryKnob stereoKnob{ "Stereo",      audioProcessor.apvts, stereoParamID };
    RotaryKnob lowCutKnob{ "Low Cut",     audioProcessor.apvts, lowCutParamID };
    RotaryKnob highCutKnob{ "High Cut",    audioProcessor.apvts, highCutParamID };
    RotaryKnob qFactorKnob{ "Q Factor",    audioProcessor.apvts, qFactorParamID };
    RotaryKnob driveKnob{ "Drive",       audioProcessor.apvts, driveParamID };
    RotaryKnob delayNoteKnob{ "Note",        audioProcessor.apvts, delayNoteParamID };

    // Buttons
    juce::TextButton tempoSyncButton;
    juce::ImageButton bypassButton;

    juce::AudioProcessorValueTreeState::ButtonAttachment tempoSyncAttachment{
        audioProcessor.apvts, tempoSyncParamID.getParamID(), tempoSyncButton
    };

    juce::AudioProcessorValueTreeState::ButtonAttachment bypassAttachment{
        audioProcessor.apvts, bypassParamID.getParamID(), bypassButton
    };

    // UI Groups
    juce::GroupComponent delayGroup;
    juce::GroupComponent feedbackGroup;
    juce::GroupComponent outputGroup;

    // UI Components
    LevelMeter meter;
    Gui::PresetPanel presetPanel;
    MainLookAndFeel mainLF;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayAudioProcessorEditor)
};
