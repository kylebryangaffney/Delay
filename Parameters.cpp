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

static float millisecondsFromString(const juce::String& text)
{
    float value = text.getFloatValue();

    if (!text.endsWithIgnoreCase("ms"))
    {
        if (text.endsWithIgnoreCase("s") || value < Parameters::minDelayTime)
        {
            return value * 1000.f;
        }
    }
    return value;
}

static juce::String stringFromDecibels(float value, int)
{
    return juce::String(value, 1) + " dB";
}

static juce::String stringFromPercent(float value, int)
{
    return juce::String(int(value)) + " %";
}

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts)
{
    castParameter(apvts, gainParamID, gainParam);
    castParameter(apvts, delayTimeParamID, delayTimeParam);
    castParameter(apvts, mixParamID, mixParam);
    castParameter(apvts, feedbackParamID, feedbackParam);
}

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>
        (
            gainParamID,
            "Output Gain",
            juce::NormalisableRange<float> {-18.f, 12.f},
            0.f,
            juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromDecibels)
        ));

    layout.add(std::make_unique<juce::AudioParameterFloat>
        (
            delayTimeParamID,
            "Delay Time",
            juce::NormalisableRange<float> {minDelayTime, maxDelayTime, 0.001f, 0.25f},
            100.f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(stringFromMilliseconds)
            .withValueFromStringFunction(millisecondsFromString)
        ));

    layout.add(std::make_unique<juce::AudioParameterFloat>
        (
            mixParamID,
            "Mix",
            juce::NormalisableRange<float>(0.f, 100.f, 1.f),
            100.f,
            juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)
        ));

    layout.add(std::make_unique<juce::AudioParameterFloat>
        (
        feedbackParamID,
        "Feedback",
        juce::NormalisableRange<float>(-90.0f, 90.0f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction(stringFromPercent)
    ));

    return layout;
}

void Parameters::prepareToPlay(double sampleRate) noexcept
{
    double duration = 0.02;
    gainSmoother.reset(sampleRate, duration);

    coeff = 1.f - std::exp(-1.f / (tau * float(sampleRate)));

    mixSmoother.reset(sampleRate, duration);

    feedbackSmoother.reset(sampleRate, duration);
}

void Parameters::reset() noexcept
{
    gain = 0.f;
    gainSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
    delayTime = 0.f;

    mix = 1.f;
    mixSmoother.setCurrentAndTargetValue(mixParam->get() * 0.01f);

    feedback = 0.f;
    feedbackSmoother.setCurrentAndTargetValue(feedbackParam->get() * 0.01f);
}

void Parameters::update() noexcept
{
    gainSmoother.setTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
    
    targetDelayTime = delayTimeParam->get();
    if (delayTime == 0.f)
    {
        delayTime = targetDelayTime;
    }

    mixSmoother.setTargetValue(mixParam->get() * 0.01f);

    feedbackSmoother.setTargetValue(feedbackParam->get() * 0.01f);

}

void Parameters::smoothen() noexcept
{
    gain = gainSmoother.getNextValue();
    delayTime += (targetDelayTime - delayTime) * coeff;

    mix = mixSmoother.getNextValue();

    feedback = feedbackSmoother.getNextValue();
}