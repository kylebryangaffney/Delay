#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>
#include "DelayLine.h"
#include "Parameters.h"
#include "Measurement.h"
#include "Service/PresetManager.h"
#include "Tempo.h"



class DelayAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    DelayAudioProcessor();
    ~DelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    juce::AudioProcessorParameter* getBypassParameter() const override;

    juce::AudioProcessorValueTreeState apvts
    {
        *this, nullptr, "Parameters", Parameters::createParameterLayout()
    };

    Parameters params;

    Measurement levelL, levelR;

    Service::PresetManager& getPresetManager() { return *presetManager; }


private:
    //==============================================================================

    float feedbackL = 0.f;
    float feedbackR = 0.f;
    juce::dsp::StateVariableTPTFilter<float> lowCutFilter;
    juce::dsp::StateVariableTPTFilter<float> highCutFilter;

    float lastLowCut = -1.f;
    float lastHighCut = -1.f;
    float lastQFactor = -1.f;
    float delayInSamples = 0.f;
    float targetDelay = 0.f;
    float fade = 0.f;
    float fadeTarget = 0.f;
    float coeff = 0.f;
    float wait = 0.f;
    float waitInc = 0.f;
    float drive = 0.f;

    float bypassFade = 1.0f;
    float bypassFadeInc = 0.0f;
    bool isBypassing = false;

    Tempo tempo;

    juce::dsp::WaveShaper<float> waveShaper;

    DelayLine delayLineL, delayLineR;

    std::unique_ptr<Service::PresetManager> presetManager;


    void initializeProcessing(juce::AudioBuffer<float>& buffer);
    void updateBypassState();
    float updateDelayTime();
    void processDelay(juce::AudioBuffer<float>& buffer,
        juce::AudioBuffer<float>& mainInput, juce::AudioBuffer<float>& mainOutput,
        float delayInSamples);
    void processDelayChannel(const float* input, float* output,
        juce::dsp::DelayLine<float>& delayLine,
        int channelIndex, float& feedbackSample,
        float delayInSamples, float& maxLevel);

    void updateFilters();
    void processFilters(float sample, int channel);
    void updateGainFade();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayAudioProcessor)
};