/*
  ==============================================================================

    Parameters.cpp
    Created: 16 Feb 2025 9:29:51am
    Author:  kyleb

  ==============================================================================
*/

#include "Parameters.h"

template<typename T>
static void castParameter(juce::AudioProcessorValueTreeState& apvts,
    const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(destination);
}

static juce::String stringFromMilliseconds(float value, int)
{
    if (value < 10.f)
    {
        return juce::String(value, 2) + " ms";
    }
    else if (value < 100.f)
    {
        return juce::String(value, 1) + " ms";
    }
    else if (value < 1000.f)
    {
        return juce::String(int(value)) + " ms";
    }
    else
    {
        return juce::String(value * 0.001f, 2) + " S";
    }
}

static juce::String stringFromDecibels(float value, int)
{
    return juce::String(value, 1) + " dB";
}

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts)
{
    castParameter(apvts, gainParamID, gainParam);
    castParameter(apvts, delayTimeParamID, delayTimeParam);
}

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>
        (
            gainParamID,
            "Output Gain",
            juce::NormalisableRange<float> {-12.f, 12.f},
            0.f,
            juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromDecibels)
        ));

    layout.add(std::make_unique<juce::AudioParameterFloat>
        (
            delayTimeParamID,
            "Delay Time",
            juce::NormalisableRange<float> {minDelayTime, maxDelayTime, 0.001f, 0.25f},
            100.f,
            juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromMilliseconds)
        ));

    return layout;
}

void Parameters::prepareToPlay(double sampleRate) noexcept
{
    double duration = 0.02;
    gainSmoother.reset(sampleRate, duration);
}

void Parameters::reset() noexcept
{
    gain = 0.f;
    gainSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
    delayTime = 0.f;
}

void Parameters::update() noexcept
{
    gainSmoother.setTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
    delayTime = delayTimeParam->get();
}

void Parameters::smoothen() noexcept
{
    gain = gainSmoother.getNextValue();
}