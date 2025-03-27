/*
  ==============================================================================

    AsymmetricalRotaryKnob.cpp
    Created: 19 Mar 2025 9:36:34am
    Author:  kyleb

  ==============================================================================
*/

#include "../Gui/AsymmetricalRotaryKnob.h"

//==============================================================================
AsymmetricalRotaryKnob::AsymmetricalRotaryKnob(const juce::String& text,
    juce::AudioProcessorValueTreeState& apvts,
    const juce::ParameterID& parameterID)
    : RotaryKnob(text, apvts, parameterID, false) // `drawFromMiddle` not needed
{
    const float pi = juce::MathConstants<float>::pi;

    // Asymmetric sweep: 225° to 450° (a bit less than full circle)
    slider.setRotaryParameters(1.25f * pi, 2.5f * pi, true);
}
