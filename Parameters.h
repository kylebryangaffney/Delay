/*
  ==============================================================================

    Parameters.h
    Created: 16 Feb 2025 9:29:51am
    Author:  kyleb

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Parameters.h"

const juce::ParameterID gainParamID{ "gain", 1 };
const juce::ParameterID delayTimeParamID{ "delayTime", 1 };
const juce::ParameterID mixParamID{ "mix", 1 };
const juce::ParameterID feedbackParamID{ "feedback", 1 };


class Parameters
{
public:

    Parameters(juce::AudioProcessorValueTreeState& apvts);
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void update() noexcept;
    void prepareToPlay(double sampleRate) noexcept;
    void reset() noexcept;
    void smoothen() noexcept;

    float gain = 0.f;
    float delayTime = 0.f;

    static constexpr float minDelayTime = 1.f;
    static constexpr float maxDelayTime = 2500.f;

    float mix = 1.f;
    float feedback = 0.f;


private:

    juce::AudioParameterFloat* gainParam;
    juce::AudioParameterFloat* delayTimeParam;
    juce::LinearSmoothedValue<float> gainSmoother;
    juce::AudioParameterFloat* feedbackParam;
    juce::LinearSmoothedValue<float> feedbackSmoother;


    float targetDelayTime = 0.f;
    float tau = 0.1f; // 100ms for parameters::prepareToPlay()
    float coeff = 0.f; // one pole smoothing 

    juce::AudioParameterFloat* mixParam;
    juce::LinearSmoothedValue<float> mixSmoother;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters);

};
