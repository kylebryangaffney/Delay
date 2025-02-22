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

    RotaryKnob gainKnob {"Output Gain", audioProcessor.apvts, gainParamID ,true};
    RotaryKnob mixKnob{ "Mix", audioProcessor.apvts, mixParamID };
    RotaryKnob delayTimeKnob{ "Delay Time", audioProcessor.apvts, delayTimeParamID };

    juce::GroupComponent delayGroup, feedbackGroup, outputGroup;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
