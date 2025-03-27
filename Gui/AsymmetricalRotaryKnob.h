/*
  ==============================================================================

    AsymmetricalRotaryKnob.h
    Created: 19 Mar 2025 9:36:34am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include "../Gui/RotaryKnob.h"

//==============================================================================
class AsymmetricalRotaryKnob : public RotaryKnob
{
public:
    AsymmetricalRotaryKnob(const juce::String& text,
        juce::AudioProcessorValueTreeState& apvts,
        const juce::ParameterID& parameterID);
};
