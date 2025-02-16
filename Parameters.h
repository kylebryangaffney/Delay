/*
  ==============================================================================

    Parameters.h
    Created: 16 Feb 2025 9:29:51am
    Author:  kyleb

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

const juce::ParameterID gainParamID{ "gain", 1 };


class Parameters
{
public:

    Parameters(juce::AudioProcessorValueTreeState& apvts);
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void update() noexcept;
    float gain = 0.f;

private:
    juce::AudioParameterFloat* gainParam;
};
