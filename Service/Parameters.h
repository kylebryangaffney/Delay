/*
  ==============================================================================

    Parameters.h
    Created: 16 Feb 2025 9:29:51am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
const juce::ParameterID gainParamID{ "gain",       1 };
const juce::ParameterID delayTimeParamID{ "delayTime",  1 };
const juce::ParameterID mixParamID{ "mix",        1 };
const juce::ParameterID feedbackParamID{ "feedback",   1 };
const juce::ParameterID stereoParamID{ "stereo",     1 };
const juce::ParameterID lowCutParamID{ "lowCut",     1 };
const juce::ParameterID highCutParamID{ "highCut",    1 };
const juce::ParameterID qFactorParamID{ "qFactor",    1 };
const juce::ParameterID driveParamID{ "drive",      1 };
const juce::ParameterID tempoSyncParamID{ "tempoSync",  1 };
const juce::ParameterID delayNoteParamID{ "delayNote",  1 };
const juce::ParameterID bypassParamID{ "bypass",     1 };

//==============================================================================
class Parameters
{
public:
    Parameters(juce::AudioProcessorValueTreeState& apvts);
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void update() noexcept;
    void prepareToPlay(double sampleRate) noexcept;
    void reset() noexcept;
    void smoothen() noexcept;

    //==============================================================================
    static constexpr float minDelayTime = 1.f;
    static constexpr float maxDelayTime = 5000.f;
    const float driveInputScale = 0.9f;

    float gain = 0.f;
    float delayTime = 0.f;
    float mix = 1.f;
    float feedback = 0.f;
    float panL = 0.f;
    float panR = 1.f;
    float stereo = 0.f;
    float lowCut = 20.f;
    float highCut = 20000.f;
    float qFactor = 0.707f;
    float drive = 0.f;
    int   delayNote = 0;
    bool  tempoSync = false;
    bool  bypassed = false;

    //==============================================================================
    juce::AudioParameterBool* tempoSyncParam = nullptr;
    juce::AudioParameterBool* bypassParam = nullptr;

private:
    // Parameter references
    juce::AudioParameterFloat* gainParam = nullptr;
    juce::AudioParameterFloat* delayTimeParam = nullptr;
    juce::AudioParameterFloat* mixParam = nullptr;
    juce::AudioParameterFloat* feedbackParam = nullptr;
    juce::AudioParameterFloat* stereoParam = nullptr;
    juce::AudioParameterFloat* lowCutParam = nullptr;
    juce::AudioParameterFloat* highCutParam = nullptr;
    juce::AudioParameterFloat* qFactorParam = nullptr;
    juce::AudioParameterFloat* driveParam = nullptr;
    juce::AudioParameterChoice* delayNoteParam = nullptr;

    // Smoothers
    juce::LinearSmoothedValue<float> gainSmoother;
    juce::LinearSmoothedValue<float> mixSmoother;
    juce::LinearSmoothedValue<float> feedbackSmoother;
    juce::LinearSmoothedValue<float> stereoSmoother;
    juce::LinearSmoothedValue<float> lowCutSmoother;
    juce::LinearSmoothedValue<float> highCutSmoother;
    juce::LinearSmoothedValue<float> qFactorSmoother;
    juce::LinearSmoothedValue<float> driveSmoother;

    // Delay smoothing
    float targetDelayTime = 0.f;
    float tau = 0.1f;  // 100ms smoothing time
    float coeff = 0.f; // One-pole smoothing

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)
};
