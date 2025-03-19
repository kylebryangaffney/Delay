/*
  ==============================================================================

    Parameters.cpp
    Created: 16 Feb 2025 9:29:51am
    Author:  kyleb

  ==============================================================================
*/

#include "Parameters.h"
#include "DSP.h"

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

static juce::String stringFromHz(float value, int)
{
    if (value < 1000.f)
        return juce::String(int(value)) + " Hz";
    else if (value < 10000.f)
        return juce::String(value / 1000.f, 2) + " kHz";
    else
        return juce::String(value / 1000.f, 1) + " kHz";
}

static float hzFromString(const juce::String& str)
{
    float value = str.getFloatValue();

    if (value < 20.f)
        return value * 1000.f;

    return value;
}

static juce::String stringFromDecimal(float value, int)
{
    return juce::String(value, 3);
}

static float decimalFromString(const juce::String& str)
{
    float value = str.getFloatValue();
    return value;
}

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts)
{
    castParameter(apvts, gainParamID, gainParam);
    castParameter(apvts, delayTimeParamID, delayTimeParam);
    castParameter(apvts, mixParamID, mixParam);
    castParameter(apvts, feedbackParamID, feedbackParam);
    castParameter(apvts, stereoParamID, stereoParam);
    castParameter(apvts, lowCutParamID, lowCutParam);
    castParameter(apvts, highCutParamID, highCutParam);
    castParameter(apvts, qFactorParamID, qFactorParam);
    castParameter(apvts, driveParamID, driveParam);
    castParameter(apvts, tempoSyncParamID, tempoSyncParam);
    castParameter(apvts, delayNoteParamID, delayNoteParam);
    castParameter(apvts, bypassParamID, bypassParam);
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
            juce::NormalisableRange<float>(0.f, 90.0f, 1.0f),
            0.0f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(stringFromPercent)
        ));

    layout.add(std::make_unique<juce::AudioParameterFloat>
        (
            stereoParamID,
            "Stereo",
            juce::NormalisableRange<float>(0.f, 100.f, 1.f),
            0.f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(stringFromPercent)
        ));

    layout.add(std::make_unique<juce::AudioParameterFloat>
        (
            lowCutParamID,
            "Low Cut",
            juce::NormalisableRange<float>(20.f, 15000.f, 1, 0.3f),
            20.f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(stringFromHz)
            .withValueFromStringFunction(hzFromString)
        ));

    layout.add(std::make_unique<juce::AudioParameterFloat>
        (
            highCutParamID,
            "High Cut",
            juce::NormalisableRange<float>(1000.f, 20000.f, 1, 0.3f),
            20000.f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(stringFromHz)
            .withValueFromStringFunction(hzFromString)
        ));

    layout.add(std::make_unique<juce::AudioParameterFloat>
        (
            qFactorParamID,
            "Q Factor",
            juce::NormalisableRange<float>(0.5f, 10.f, 0.001f, 0.181f),
            0.707f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(stringFromDecimal)
            .withValueFromStringFunction(decimalFromString)
        ));

    layout.add(std::make_unique<juce::AudioParameterFloat>
        (
            driveParamID,
            "Drive",
            juce::NormalisableRange<float>(0.f, 100.f, 1.f),
            0.0f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(stringFromPercent)
        ));

    layout.add(std::make_unique<juce::AudioParameterBool>
        (tempoSyncParamID, "Tempo Sync", false));

    juce::StringArray noteLengths =
    {
        "1/32",
        "1/16 trip",
        "1/32 dot",
        "1/16",
        "1/8 trip",
        "1/16 dot",
        "1/8",
        "1/4 trip",
        "1/8 dot",
        "1/4",
        "1/2 trip",
        "1/4 dot",
        "1/2",
        "1/1 trip",
        "1/2 dot",
        "1/1",
    };

    layout.add(std::make_unique<juce::AudioParameterChoice>
        (delayNoteParamID, "Delay Note", noteLengths, 9));

    layout.add(std::make_unique<juce::AudioParameterBool>
        (bypassParamID, "Bypass", false));

    return layout;
}

void Parameters::prepareToPlay(double sampleRate) noexcept
{
    double duration = 0.02;
    gainSmoother.reset(sampleRate, duration);

    coeff = 1.f - std::exp(-1.f / (tau * float(sampleRate)));
    mixSmoother.reset(sampleRate, duration);
    feedbackSmoother.reset(sampleRate, duration);
    stereoSmoother.reset(sampleRate, duration);
    lowCutSmoother.reset(sampleRate, duration);
    highCutSmoother.reset(sampleRate, duration);
    qFactorSmoother.reset(sampleRate, duration);
    driveSmoother.reset(sampleRate, duration);
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
    stereoSmoother.setCurrentAndTargetValue(stereoParam->get() * 0.01f);
    panL = 0.f;
    panR = 1.f;
    lowCut = 20.f;
    lowCutSmoother.setCurrentAndTargetValue(lowCutParam->get());
    highCut = 20000.f;
    highCutSmoother.setCurrentAndTargetValue(highCutParam->get());
    qFactorSmoother.setCurrentAndTargetValue(qFactorParam->get());
    driveSmoother.setCurrentAndTargetValue(driveParam->get());
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
    stereoSmoother.setTargetValue(stereoParam->get() * 0.01f);
    lowCutSmoother.setTargetValue(lowCutParam->get());
    highCutSmoother.setTargetValue(highCutParam->get());
    qFactorSmoother.setTargetValue(qFactorParam->get());
    driveSmoother.setTargetValue(driveParam->get());
    delayNote = delayNoteParam->getIndex();
    tempoSync = tempoSyncParam->get();
    bypassed = bypassParam->get();

}

void Parameters::smoothen() noexcept
{
    gain = gainSmoother.getNextValue();
    delayTime = targetDelayTime;
    mix = mixSmoother.getNextValue();
    feedback = feedbackSmoother.getNextValue();
    panningEqualPower(stereoSmoother.getNextValue(), panL, panR);
    lowCut = lowCutSmoother.getNextValue();
    highCut = highCutSmoother.getNextValue();
    qFactor = qFactorSmoother.getNextValue();
    drive = driveSmoother.getNextValue();
}