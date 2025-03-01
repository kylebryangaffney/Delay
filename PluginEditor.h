#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RotaryKnob.h"
#include "LookAndFeel.h"

class DelayAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DelayAudioProcessorEditor (DelayAudioProcessor&);
    ~DelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

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

    juce::GroupComponent delayGroup, feedbackGroup, outputGroup;

    MainLookAndFeel mainLF;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
