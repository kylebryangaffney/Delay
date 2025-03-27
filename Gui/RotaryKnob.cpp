/*
  ==============================================================================

    RotaryKnob.cpp
    Created: 20 Feb 2025 10:03:37am
    Author:  kyleb

  ==============================================================================
*/

#include <JuceHeader.h>
#include "../Gui/RotaryKnob.h"
#include "../Gui/LookAndFeel.h"

//==============================================================================
RotaryKnob::RotaryKnob(const juce::String& text,
    juce::AudioProcessorValueTreeState& apvts,
    const juce::ParameterID& parameterID,
    bool drawFromMiddle)
    : attachment(apvts, parameterID.getParamID(), slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 16);
    slider.setBounds(0, 0, 70, 86);
    slider.setRotaryParameters(1.25f * juce::MathConstants<float>::pi,
        2.75f * juce::MathConstants<float>::pi,
        true);
    slider.getProperties().set("drawFromMiddle", drawFromMiddle);
    addAndMakeVisible(slider);

    label.setText(text, juce::NotificationType::dontSendNotification);
    label.setJustificationType(juce::Justification::horizontallyCentred);
    label.setBorderSize(juce::BorderSize<int>{ 0, 0, 2, 0 });
    label.attachToComponent(&slider, false);
    addAndMakeVisible(label);

    setLookAndFeel(RotaryKnobLookAndFeel::get());
    setSize(70, 110);
}

RotaryKnob::~RotaryKnob()
{
}

//==============================================================================
void RotaryKnob::resized()
{
    slider.setTopLeftPosition(0, 24);
}
