#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>
#include "DelayLine.h"
#include "Parameters.h"
#include "Measurement.h"
#include "Tempo.h"



class DelayAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    DelayAudioProcessor();
    ~DelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts
    {
        *this, nullptr, "Parameters", Parameters::createParameterLayout()
    };

    Parameters params;

    Measurement levelL, levelR;


private:
    //==============================================================================
   
    float feedbackL = 0.f;
    float feedbackR = 0.f;
    juce::dsp::StateVariableTPTFilter<float> lowCutFilter;
    juce::dsp::StateVariableTPTFilter<float> highCutFilter;

    float lastLowCut = -1.f;
    float lastHighCut = -1.f;
    float lastQFactor = -1.f;

    float drive = 0.f;

    Tempo tempo;

    juce::dsp::WaveShaper<float> waveShaper;

    DelayLine delayLineL, delayLineR;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessor)
};
