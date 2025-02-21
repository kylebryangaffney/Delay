/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 21 Feb 2025 9:45:38am
    Author:  kyleb

  ==============================================================================
*/

#include "LookAndFeel.h"

RotaryKnobLookAndFeel::RotaryKnobLookAndFeel()
{
    setColour(juce::Label::textColourId, Colors::Knob::label);
    setColour(juce::Slider::textBoxTextColourId, Colors::Knob::label);
}

void RotaryKnobLookAndFeel::drawRotarySlider(juce::Graphics& g, 
    int x, int y, int width, [[maybe_unused]] int height,
    float sliderPos, 
    float rotaryStartAngle, float rotaryEndAngle, 
    juce::Slider& slider)
{
    juce::Rectangle<float> bounds = juce::Rectangle<int>(x, y, width, width).toFloat();
    auto knobRect = bounds.reduced(10.f, 10.f);

    juce::Path path = juce::Path();
    path.addEllipse(knobRect);
    dropShadow.drawForPath(g, path);

    g.setColour(Colors::Knob::outline);
    g.fillEllipse(knobRect);

    juce::Rectangle<float> innerRect = knobRect.reduced(2.f, 2.f);
    juce::ColourGradient gradient = juce::ColourGradient
    (
        Colors::Knob::gradientTop, 0.f, innerRect.getY(),
        Colors::Knob::gradientBottom, 0.f, innerRect.getBottom(), false
    );
    g.setGradientFill(gradient);
    g.fillEllipse(innerRect);
}