#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>

#include "../Service/PresetManager.h"
#include "../Service/Parameters.h"
#include "../Service/Measurement.h"
#include "../DSP/Tempo.h"
#include "../DSP/DelayLine.h"

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

    //==============================================================================
    juce::AudioProcessorValueTreeState apvts{
        *this, nullptr, "Parameters", Parameters::createParameterLayout()
    };

    Parameters params;
    Measurement levelL, levelR;

    Service::PresetManager& getPresetManager() { return *presetManager; }

private:
    //==============================================================================
    // Delay and filter processing
    DelayLine delayLineL, delayLineR;
    juce::dsp::StateVariableTPTFilter<float> lowCutFilter, highCutFilter;
    juce::dsp::WaveShaper<float> waveShaper;
    Tempo tempo;

    //==============================================================================
    // Internal DSP state
    float feedbackL = 0.f;
    float feedbackR = 0.f;
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
    bool  isBypassing = false;

    //==============================================================================
    // Presets
    std::unique_ptr<Service::PresetManager> presetManager;

    //==============================================================================
    // Internal processing helpers
    void initializeProcessing(juce::AudioBuffer<float>& buffer);
    void updateBypassState();
    float getTempoSyncedDelay();
    float convertMsToSamples(float& sampleRate, float& ms);
    void updateDelayTime(float& newTargetDelay);
    void updateLowCutFilter();
    void updateHighCutFilter();
    void applyDelayCrossfade(float& wet);
    void processWetWithFeedback(float& wet, float& feedback, const int channel);
    float getWetDryOutput(const float dry, const float wet);
    void updateBypassFade();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayAudioProcessor)
};
