/*
  ==============================================================================

    DSP.h
    Created: 25 Feb 2025 9:02:31am
    Author:  kyleb

  ==============================================================================
*/

#pragma once


#include <cmath>

inline void panningEqualPower(float panning, float& left, float& right)
{
    float x = 0.7853981633974483f * (panning + 1.f);
    left = std::cos(x);
    right = std::sin(x);
}
