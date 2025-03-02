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
const juce::ParameterID delayTimeParamID{ "delayTime", 1 };
const juce::ParameterID mixParamID{ "mix", 1 };
const juce::ParameterID feedbackParamID{ "feedback", 1 };
const juce::ParameterID stereoParamID{ "stereo", 1 };
const juce::ParameterID lowCutParamID{ "lowCut", 1 };
const juce::ParameterID highCutParamID{ "highCut", 1 };
const juce::ParameterID qFactorParamID{ "qFactor", 1 };
const juce::ParameterID driveParamID{ "drive", 1 };
const juce::ParameterID tempoSyncParamID{ "tempoSync", 1 };
const juce::ParameterID delayNoteParamID{ "delayNote", 1 };

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
    float panL = 0.f;
    float panR = 1.f;
    float lowCut = 20.f;
    float highCut = 20000.f;
    float qFactor = 0.707f;
    float drive = 0.f;
    int delayNote = 0;
    bool tempoSync = false;


private:

    juce::AudioParameterFloat* gainParam;
    juce::AudioParameterFloat* delayTimeParam;
    juce::LinearSmoothedValue<float> gainSmoother;
    juce::AudioParameterFloat* mixParam;
    juce::LinearSmoothedValue<float> mixSmoother;
    juce::AudioParameterFloat* feedbackParam;
    juce::LinearSmoothedValue<float> feedbackSmoother;
    juce::AudioParameterFloat* stereoParam;
    juce::LinearSmoothedValue<float> stereoSmoother;
    juce::AudioParameterFloat* lowCutParam;
    juce::LinearSmoothedValue<float> lowCutSmoother;
    juce::AudioParameterFloat* highCutParam;
    juce::LinearSmoothedValue<float> highCutSmoother;
    juce::AudioParameterFloat* qFactorParam;
    juce::LinearSmoothedValue<float> qFactorSmoother;
    juce::AudioParameterFloat* driveParam;
    juce::LinearSmoothedValue<float> driveSmoother;
    juce::AudioParameterBool* tempoSyncParam;
    juce::AudioParameterChoice* delayNoteParam;


    float targetDelayTime = 0.f;
    float tau = 0.1f; // 100ms for parameters::prepareToPlay()
    float coeff = 0.f; // one pole smoothing 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters);

};
