#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ProtectYourEars.h"

//==============================================================================
DelayAudioProcessor::DelayAudioProcessor() :
    AudioProcessor(
        BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    ),
    params(apvts)
{
    lowCutFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    highCutFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    waveShaper.functionToUse = [](float x) { return std::tanh(x); };
    apvts.state.setProperty(Service::PresetManager::presetNameProperty, "", nullptr);
    apvts.state.setProperty("version", ProjectInfo::versionString, nullptr);
    presetManager = std::make_unique<Service::PresetManager>(apvts);
}

DelayAudioProcessor::~DelayAudioProcessor()
{
}

//==============================================================================
const juce::String DelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool DelayAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool DelayAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double DelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int DelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayAudioProcessor::setCurrentProgram(int)
{
}

const juce::String DelayAudioProcessor::getProgramName(int)
{
    return {};
}

void DelayAudioProcessor::changeProgramName(int, const juce::String&)
{
}

//==============================================================================
void DelayAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    params.prepareToPlay(sampleRate);
    params.reset();

    levelL.reset();
    levelR.reset();

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = juce::uint32(samplesPerBlock);
    spec.numChannels = 2;

    double numSamples = Parameters::maxDelayTime / 1000.0 * sampleRate;
    int maxDelayInSamples = int(std::ceil(numSamples));

    delayLineL.setMaximumDelayInSamples(maxDelayInSamples);
    delayLineR.setMaximumDelayInSamples(maxDelayInSamples);
    delayLineL.reset();
    delayLineR.reset();

    feedbackL = 0.f;
    feedbackR = 0.f;

    lowCutFilter.prepare(spec);
    lowCutFilter.reset();

    highCutFilter.prepare(spec);
    highCutFilter.reset();

    lastLowCut = -1.f;
    lastHighCut = -1.f;

    drive = 0.f;
    waveShaper.prepare(spec);
    tempo.reset();

    delayInSamples = 0.f;
    targetDelay = 0.f;
    fade = 1.f;
    fadeTarget = 1.f;
    coeff = 1.f - std::exp(-1.f / (0.08f * float(sampleRate)));
    wait = 0.f;
    waitInc = 1.f / (0.25f * float(sampleRate)); // 250ms

    bypassFade = 1.0f;
    bypassFadeInc = static_cast<float>(1.0 / (0.1 * sampleRate)); // 100ms


    DBG(maxDelayInSamples);
}

void DelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const juce::AudioChannelSet mono = juce::AudioChannelSet::mono();
    const juce::AudioChannelSet stereo = juce::AudioChannelSet::stereo();
    const juce::AudioChannelSet mainIn = layouts.getMainInputChannelSet();
    const juce::AudioChannelSet mainOut = layouts.getMainOutputChannelSet();

    DBG("isBusesLayoutSupported, in: " << mainIn.getDescription()
        << ", out: " << mainOut.getDescription());

    if (mainIn == mono && mainOut == mono)
        return true;
    if (mainIn == mono && mainOut == stereo)
        return true;
    if (mainIn == stereo && mainOut == stereo)
        return true;

    return false;
}
#endif


void DelayAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    int totalNumInputChannels = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    params.update();

    if (params.bypassed && bypassFade >= 1.0f)
    {
        return;
    }

    if (params.bypassed && bypassFade < 1.0f)
        bypassFade += bypassFadeInc;

    else if (!params.bypassed && bypassFade > 0.0f)
        bypassFade -= bypassFadeInc;

    tempo.update(getPlayHead());

    float syncedTime = float(tempo.getMillisecondsForNoteLength(params.delayNote));
    if (syncedTime > Parameters::maxDelayTime)
    {
        syncedTime = Parameters::maxDelayTime;
    }

    float sampleRate = float(getSampleRate());

    float delayTime = params.tempoSync ? syncedTime : params.delayTime;
    float newTargetDelay = delayTime / 1000.f * sampleRate;

    if (newTargetDelay != targetDelay)
    {
        targetDelay = newTargetDelay;

        if (delayInSamples == 0.f) // first iteration
        {
            delayInSamples = targetDelay;
        }
        else //start counter and fade out
        {
            wait = waitInc;
            fadeTarget = 0.f;
        }
    }

    juce::AudioBuffer mainInput = getBusBuffer(buffer, true, 0);
    int mainInputChannels = mainInput.getNumChannels();
    bool isMainInputStereo = mainInputChannels > 1;
    const float* inputDataL = mainInput.getReadPointer(0);
    const float* inputDataR = mainInput.getReadPointer(isMainInputStereo ? 1 : 0);

    juce::AudioBuffer mainOutput = getBusBuffer(buffer, false, 0);
    int mainOutputChannels = mainOutput.getNumChannels();
    bool isMainOutputStereo = mainOutputChannels > 1;
    float* outputDataL = mainOutput.getWritePointer(0);
    float* outputDataR = mainOutput.getWritePointer(isMainOutputStereo ? 1 : 0);

    float maxL = 0.f;
    float maxR = 0.f;

    if (isMainOutputStereo)
    {
        drive = params.drive;

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            params.smoothen();
            float delayInSamples = params.delayTime / 1000.0f * sampleRate;

            if (params.lowCut != lastLowCut || params.qFactor != lastQFactor)
            {
                lowCutFilter.setResonance(params.qFactor);
                lowCutFilter.setCutoffFrequency(params.lowCut);
                lastLowCut = params.lowCut;
            }

            if (params.highCut != lastHighCut || params.qFactor != lastQFactor)
            {
                highCutFilter.setResonance(params.qFactor);
                highCutFilter.setCutoffFrequency(params.highCut);
                lastHighCut = params.highCut;
            }

            lastQFactor = params.qFactor;

            float dryL = inputDataL[sample];
            float dryR = inputDataR[sample];

            float mono = (dryL + dryR) * 0.5f;

            delayLineL.write(mono * params.panL + feedbackR);
            delayLineR.write(dryR * params.panR + feedbackL);

            float wetL = delayLineL.read(delayInSamples);
            float wetR = delayLineR.read(delayInSamples);

            fade += (fadeTarget - fade) * coeff;
            wetL *= fade;
            wetR *= fade;

            if (wait > 0.f)
            {
                wait += waitInc;
                if (wait >= 1.f)
                {
                    delayInSamples = targetDelay;
                    wait = 0.f;
                    fadeTarget = 1.f;
                }
            }

            feedbackL = wetL * params.feedback;
            wetL = lowCutFilter.processSample(0, wetL);
            wetL = highCutFilter.processSample(0, wetL);

            feedbackR = wetR * params.feedback;
            wetR = lowCutFilter.processSample(1, wetR);
            wetR = highCutFilter.processSample(1, wetR);

            if (drive > 0)
            {
                wetL = waveShaper.processSample(wetL * drive * 0.9f);
                wetR = waveShaper.processSample(wetR * drive * 0.9f);
            }

            float mixL = dryL * (1.f - params.mix) + wetL * params.mix;
            float mixR = dryR * (1.f - params.mix) + wetR * params.mix;

            float outL = mixL * params.gain;
            float outR = mixR * params.gain;

            outL = (1.0f - bypassFade) * outL + bypassFade * dryL;
            outR = (1.0f - bypassFade) * outR + bypassFade * dryR;

            if (params.bypassed && bypassFade < 1.0f)
                bypassFade += bypassFadeInc;

            else if (!params.bypassed && bypassFade > 0.0f)
                bypassFade -= bypassFadeInc;

            outputDataL[sample] = outL;
            outputDataR[sample] = outR;
            maxL = std::max(maxL, std::abs(outL));
            maxR = std::max(maxR, std::abs(outR));

        }
    }
    else
    {
        drive = params.drive;

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            params.smoothen();
            float delayInSamples = params.delayTime / 1000.0f * sampleRate;

            if (params.lowCut != lastLowCut || params.qFactor != lastQFactor)
            {
                lowCutFilter.setResonance(params.qFactor);
                lowCutFilter.setCutoffFrequency(params.lowCut);
                lastLowCut = params.lowCut;
            }

            if (params.highCut != lastHighCut || params.qFactor != lastQFactor)
            {
                highCutFilter.setResonance(params.qFactor);
                highCutFilter.setCutoffFrequency(params.highCut);
                lastHighCut = params.highCut;
            }

            lastQFactor = params.qFactor;

            float dryL = inputDataL[sample];

            delayLineL.write(dryL * params.panL + feedbackR);

            float wetL = delayLineL.read(delayInSamples);

            fade += (fadeTarget - fade) * coeff;
            wetL *= fade;

            if (wait > 0.f)
            {
                wait += waitInc;
                if (wait >= 1.f)
                {
                    delayInSamples = targetDelay;
                    wait = 0.f;
                    fadeTarget = 1.f;
                }
            }

            feedbackL = wetL * params.feedback;
            wetL = lowCutFilter.processSample(0, wetL);
            wetL = highCutFilter.processSample(0, wetL);

            if (drive > 0)
            {
                wetL = waveShaper.processSample(wetL * drive * 0.9f);
            }

            float mixL = dryL * (1.f - params.mix) + wetL * params.mix;

            float outL = mixL * params.gain;

            outL = (1.0f - bypassFade) * outL + bypassFade * dryL;

            if (params.bypassed && bypassFade < 1.0f)
                bypassFade += bypassFadeInc;

            else if (!params.bypassed && bypassFade > 0.0f)
                bypassFade -= bypassFadeInc;

            outputDataL[sample] = outL;
            outputDataR[sample] = outL;
            maxL = std::max(maxL, std::abs(outL));
            maxR = std::max(maxR, std::abs(outL));

        }
    }

    levelL.updateIfGreater(maxL);
    levelR.updateIfGreater(maxR);

#if JUCE_DEBUG
    protectYourEars(buffer);
#endif

}


bool DelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DelayAudioProcessor::createEditor()
{
    return new DelayAudioProcessorEditor(*this);
}

//==============================================================================
void DelayAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
    //DBG(apvts.copyState().toXmlString());
}

void DelayAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType()))
    {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

juce::AudioProcessorParameter* DelayAudioProcessor::getBypassParameter() const
{
    return params.bypassParam;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}