/*
  ==============================================================================

    AsymmetricalRotaryKnob.cpp
    Created: 19 Mar 2025 9:36:34am
    Author:  kyleb

  ==============================================================================
*/

#include "AsymmetricalRotaryKnob.h"


AsymmetricalRotaryKnob::AsymmetricalRotaryKnob(const juce::String& text,
    juce::AudioProcessorValueTreeState& apvts,
    const juce::ParameterID& parameterID)
    : RotaryKnob(text, apvts, parameterID, false) // `drawFromMiddle` not needed
{
    float pi = juce::MathConstants<float>::pi;

    // Adjust rotary parameters for asymmetric behavior
    slider.setRotaryParameters(1.25f * pi, 2.5f * pi, true);
}
