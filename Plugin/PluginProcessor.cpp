#include "../Plugin/PluginProcessor.h"
#include "../Plugin/PluginEditor.h"
#include "../Service/ProtectYourEars.h"

//==============================================================================
DelayAudioProcessor::DelayAudioProcessor()
    : AudioProcessor(
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

DelayAudioProcessor::~DelayAudioProcessor() {}

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

int DelayAudioProcessor::getNumPrograms() { return 1; }
int DelayAudioProcessor::getCurrentProgram() { return 0; }
void DelayAudioProcessor::setCurrentProgram(int) {}
const juce::String DelayAudioProcessor::getProgramName(int) { return {}; }
void DelayAudioProcessor::changeProgramName(int, const juce::String&) {}

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
    waitInc = 1.f / (0.25f * float(sampleRate));

    bypassFade = 1.0f;
    bypassFadeInc = static_cast<float>(1.0 / (0.1 * sampleRate));

    DBG(maxDelayInSamples);
}

void DelayAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto mono = juce::AudioChannelSet::mono();
    const auto stereo = juce::AudioChannelSet::stereo();
    const auto in = layouts.getMainInputChannelSet();
    const auto out = layouts.getMainOutputChannelSet();

    DBG("isBusesLayoutSupported, in: " << in.getDescription() << ", out: " << out.getDescription());

    if ((in == mono && out == mono) ||
        (in == mono && out == stereo) ||
        (in == stereo && out == stereo))
        return true;

    return false;
}
#endif

void DelayAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages)
{
    initializeProcessing(buffer);
    updateBypassState();

    tempo.update(getPlayHead());
    float syncedTime = getTempoSyncedDelay();
    float sampleRate = float(getSampleRate());
    float delayTime = params.tempoSync ? syncedTime : params.delayTime;
    float newTargetDelay = convertMsToSamples(sampleRate, delayTime);

    updateDelayTime(newTargetDelay);

    juce::AudioBuffer mainInput = getBusBuffer(buffer, true, 0);
    juce::AudioBuffer mainOutput = getBusBuffer(buffer, false, 0);

    const bool stereoIn = mainInput.getNumChannels() > 1;
    const bool stereoOut = mainOutput.getNumChannels() > 1;

    const float* inputDataL = mainInput.getReadPointer(0);
    const float* inputDataR = mainInput.getReadPointer(stereoIn ? 1 : 0);
    float* outputDataL = mainOutput.getWritePointer(0);
    float* outputDataR = mainOutput.getWritePointer(stereoOut ? 1 : 0);

    float maxL = 0.f;
    float maxR = 0.f;

    drive = params.drive;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        params.smoothen();
        float delaySamples = params.delayTime / 1000.0f * sampleRate;

        updateLowCutFilter();
        updateHighCutFilter();
        lastQFactor = params.qFactor;

        float dryL = inputDataL[sample];
        float dryR = stereoOut ? inputDataR[sample] : dryL;

        float stereo = params.stereo;
        float pingPongFeedbackL = feedbackR * stereo + feedbackL * (1.0f - stereo);
        float pingPongFeedbackR = feedbackL * stereo + feedbackR * (1.0f - stereo);

        delayLineL.write(dryL * params.panL + pingPongFeedbackL);
        delayLineR.write(dryR * params.panR + pingPongFeedbackR);

        float wetL = delayLineL.read(delaySamples);
        float wetR = delayLineR.read(delaySamples);

        applyDelayCrossfade(wetL);
        applyDelayCrossfade(wetR);

        processWetWithFeedback(wetL, feedbackL, 0);
        processWetWithFeedback(wetR, feedbackR, 1);

        if (drive > 0)
        {
            wetL = waveShaper.processSample(wetL * drive * params.driveInputScale);
            wetR = waveShaper.processSample(wetR * drive * params.driveInputScale);
        }

        float outL = getWetDryOutput(dryL, wetL);
        float outR = getWetDryOutput(dryR, wetR);

        updateBypassFade();

        outputDataL[sample] = outL;
        outputDataR[sample] = outR;

        maxL = std::max(maxL, std::abs(outL));
        maxR = std::max(maxR, std::abs(outR));
    }

    levelL.updateIfGreater(maxL);
    levelR.updateIfGreater(maxR);

#if JUCE_DEBUG
    protectYourEars(buffer);
#endif
}

//==============================================================================
bool DelayAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* DelayAudioProcessor::createEditor()
{
    return new DelayAudioProcessorEditor(*this);
}

//==============================================================================
void DelayAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void DelayAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorParameter* DelayAudioProcessor::getBypassParameter() const
{
    return params.bypassParam;
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}

//==============================================================================
void DelayAudioProcessor::initializeProcessing(juce::AudioBuffer<float>& buffer)
{
    juce::ScopedNoDenormals noDenormals;

    int totalNumInputChannels = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    params.update();
}

void DelayAudioProcessor::updateBypassState()
{
    if (params.bypassed && bypassFade >= 1.0f)
        return;

    if (params.bypassed && bypassFade < 1.0f)
        bypassFade += bypassFadeInc;
    else if (!params.bypassed && bypassFade > 0.0f)
        bypassFade -= bypassFadeInc;
}

float DelayAudioProcessor::getTempoSyncedDelay()
{
    tempo.update(getPlayHead());

    float syncedTime = float(tempo.getMillisecondsForNoteLength(params.delayNote));
    return std::min(syncedTime, Parameters::maxDelayTime);
}

float DelayAudioProcessor::convertMsToSamples(float& sampleRate, float& ms)
{
    return ms / 1000.f * sampleRate;
}

void DelayAudioProcessor::updateDelayTime(float& newTargetDelay)
{
    if (newTargetDelay != targetDelay)
    {
        targetDelay = newTargetDelay;

        if (delayInSamples == 0.f)
        {
            delayInSamples = targetDelay;
        }
        else
        {
            wait = waitInc;
            fadeTarget = 0.f;
        }
    }
}

void DelayAudioProcessor::updateLowCutFilter()
{
    if (params.lowCut != lastLowCut || params.qFactor != lastQFactor)
    {
        lowCutFilter.setResonance(params.qFactor);
        lowCutFilter.setCutoffFrequency(params.lowCut);
        lastLowCut = params.lowCut;
    }
}

void DelayAudioProcessor::updateHighCutFilter()
{
    if (params.highCut != lastHighCut || params.qFactor != lastQFactor)
    {
        highCutFilter.setResonance(params.qFactor);
        highCutFilter.setCutoffFrequency(params.highCut);
        lastHighCut = params.highCut;
    }
}

void DelayAudioProcessor::applyDelayCrossfade(float& wet)
{
    fade += (fadeTarget - fade) * coeff;
    wet *= fade;

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
}

void DelayAudioProcessor::processWetWithFeedback(float& wet, float& feedback, const int channel)
{
    feedback = wet * params.feedback;
    wet = lowCutFilter.processSample(channel, wet);
    wet = highCutFilter.processSample(channel, wet);
}

float DelayAudioProcessor::getWetDryOutput(const float dry, const float wet)
{
    float mix = dry * (1.f - params.mix) + wet * params.mix;
    float out = mix * params.gain;
    return (1.0f - bypassFade) * out + bypassFade * dry;
}

void DelayAudioProcessor::updateBypassFade()
{
    if (params.bypassed && bypassFade < 1.0f)
        bypassFade += bypassFadeInc;
    else if (!params.bypassed && bypassFade > 0.0f)
        bypassFade -= bypassFadeInc;
}
