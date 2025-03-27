/*
  ==============================================================================

    DSP.h
    Created: 25 Feb 2025 9:02:31am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <cmath>

//==============================================================================
// Equal-power panning curve.
// panning range: -1.0 (full left) to 1.0 (full right)
inline void panningEqualPower(float panning, float& left, float& right)
{
    constexpr float halfPi = 0.5f * juce::MathConstants<float>::pi;
    float angle = halfPi * (panning + 1.0f); // Map [-1, 1] to [0, pi]
    left = std::cos(angle);
    right = std::sin(angle);
}
