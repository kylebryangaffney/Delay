#pragma once

#include "RotaryKnob.h"

class AsymmetricalRotaryKnob : public RotaryKnob
{
public:
    AsymmetricalRotaryKnob(const juce::String& text,
        juce::AudioProcessorValueTreeState& apvts,
        const juce::ParameterID& parameterID);
};
