#include "../Plugin/PluginProcessor.h"
#include "../Plugin/PluginEditor.h"

//==============================================================================
DelayAudioProcessorEditor::DelayAudioProcessorEditor(DelayAudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),
    meter(p.levelL, p.levelR),
    presetPanel(p.getPresetManager())
{
    setLookAndFeel(&mainLF);

    // Delay group
    delayGroup.setText("Delay");
    delayGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    delayGroup.addAndMakeVisible(delayTimeKnob);
    delayGroup.addChildComponent(delayNoteKnob);

    tempoSyncButton.setButtonText("Sync");
    tempoSyncButton.setClickingTogglesState(true);
    tempoSyncButton.setBounds(0, 0, 70, 27);
    tempoSyncButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
    tempoSyncButton.setLookAndFeel(ButtonLookAndFeel::get());
    delayGroup.addAndMakeVisible(tempoSyncButton);

    addAndMakeVisible(delayGroup);

    // Feedback group
    feedbackGroup.setText("Feedback");
    feedbackGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    feedbackGroup.addAndMakeVisible(feedbackKnob);
    feedbackGroup.addAndMakeVisible(stereoKnob);
    feedbackGroup.addAndMakeVisible(lowCutKnob);
    feedbackGroup.addAndMakeVisible(highCutKnob);
    feedbackGroup.addAndMakeVisible(qFactorKnob);
    feedbackGroup.addAndMakeVisible(driveKnob);
    addAndMakeVisible(feedbackGroup);

    // Output group
    outputGroup.setText("Output");
    outputGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    outputGroup.addAndMakeVisible(gainKnob);
    outputGroup.addAndMakeVisible(mixKnob);
    outputGroup.addAndMakeVisible(meter);
    addAndMakeVisible(outputGroup);

    // Bypass button
    auto bypassIcon = juce::ImageCache::getFromMemory(BinaryData::Bypass_png, BinaryData::Bypass_pngSize);
    bypassButton.setClickingTogglesState(true);
    bypassButton.setBounds(0, 0, 20, 20);
    bypassButton.setImages(false, true, true,
        bypassIcon, 1.f, juce::Colours::white,
        bypassIcon, 1.f, juce::Colours::white,
        bypassIcon, 1.f, juce::Colours::grey,
        0.f);
    addAndMakeVisible(bypassButton);

    // Presets + finishing touches
    addAndMakeVisible(presetPanel);
    gainKnob.slider.setColour(juce::Slider::rotarySliderFillColourId, Colors::Slider::standoutFill);

    setSize(500, 490);

    updateDelayKnobs(audioProcessor.params.tempoSyncParam->get());
    audioProcessor.params.tempoSyncParam->addListener(this);
}

DelayAudioProcessorEditor::~DelayAudioProcessorEditor()
{
    audioProcessor.params.tempoSyncParam->removeListener(this);
    setLookAndFeel(nullptr);
}

//==============================================================================
void DelayAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(Colors::background);

    auto rect = getLocalBounds().withHeight(40);
    g.setColour(Colors::header);
    g.fillRect(rect);

    auto image = juce::ImageCache::getFromMemory(BinaryData::Logo_png, BinaryData::Logo_pngSize);
    int destWidth = image.getWidth() / 2;
    int destHeight = image.getHeight() / 2;

    g.drawImage(image,
        getWidth() / 2 - destWidth / 2, 0,
        destWidth, destHeight,
        0, 0, image.getWidth(), image.getHeight());
}

void DelayAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    int headerHeight = 50;
    int presetPanelHeight = 40;
    int padding = 10;
    int y = headerHeight + presetPanelHeight + padding;

    presetPanel.setBounds(bounds.getX() + 10, headerHeight, bounds.getWidth() - 20, presetPanelHeight);

    delayGroup.setBounds(10, y, 110, bounds.getHeight() - y - padding);
    outputGroup.setBounds(bounds.getWidth() - 160, y, 150, bounds.getHeight() - y - padding);
    feedbackGroup.setBounds(delayGroup.getRight() + 10, y, outputGroup.getX() - delayGroup.getRight() - 20, bounds.getHeight() - y - padding);

    delayTimeKnob.setTopLeftPosition(20, 20);
    tempoSyncButton.setTopLeftPosition(20, delayTimeKnob.getBottom() + 10);
    delayNoteKnob.setTopLeftPosition(delayTimeKnob.getX(), delayTimeKnob.getY());

    mixKnob.setTopLeftPosition(20, 20);
    gainKnob.setTopLeftPosition(mixKnob.getX(), mixKnob.getBottom() + 10);

    feedbackKnob.setTopLeftPosition(20, 20);
    stereoKnob.setTopLeftPosition(feedbackKnob.getRight() + 20, 20);
    lowCutKnob.setTopLeftPosition(feedbackKnob.getX(), feedbackKnob.getBottom() + 10);
    highCutKnob.setTopLeftPosition(lowCutKnob.getRight() + 20, lowCutKnob.getY());
    qFactorKnob.setTopLeftPosition(lowCutKnob.getX(), lowCutKnob.getBottom() + 10);
    driveKnob.setTopLeftPosition(qFactorKnob.getRight() + 20, lowCutKnob.getBottom() + 10);

    meter.setBounds(outputGroup.getWidth() - 45, 30, 30, gainKnob.getBottom() - 30);

    bypassButton.setTopLeftPosition(bounds.getWidth() - bypassButton.getWidth() - 10, 10);
}

void DelayAudioProcessorEditor::parameterValueChanged(int, float value)
{
    if (juce::MessageManager::getInstance()->isThisTheMessageThread())
    {
        updateDelayKnobs(value != 0.f);
    }
    else
    {
        juce::MessageManager::callAsync([this, value]
            {
                updateDelayKnobs(value != 0.f);
            });
    }
}

void DelayAudioProcessorEditor::updateDelayKnobs(bool tempoSyncActive)
{
    delayTimeKnob.setVisible(!tempoSyncActive);
    delayNoteKnob.setVisible(tempoSyncActive);
}
