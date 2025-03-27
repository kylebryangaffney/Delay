/*
  ==============================================================================

    LevelMeter.h
    Created: 7 Mar 2025 8:58:15am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Service/Measurement.h"

//==============================================================================
class LevelMeter : public juce::Component,
    private juce::Timer
{
public:
    LevelMeter(Measurement& measurementL, Measurement& measurementR);
    ~LevelMeter() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    Measurement& measurementL;
    Measurement& measurementR;

    // Decibel reference ranges
    static constexpr float maxdB = 6.f;
    static constexpr float mindB = -60.f;
    static constexpr float stepdB = 6.f;
    static constexpr float clampdB = -120.f;
    static constexpr float clampLevel = 0.000001f;

    // Refresh rate in Hz
    static constexpr int refreshRate = 60;

    // Meter smoothing and level values
    float decay = 0.f;
    float levelL = clampLevel;
    float levelR = clampLevel;
    float dbLevelL = 0.f;
    float dbLevelR = 0.f;
    float maxPos = 0.f;
    float minPos = 0.f;

    //==============================================================================
    void timerCallback() override;

    int positionForLevel(float dbLevel) const noexcept
    {
        return int(std::round(juce::jmap(dbLevel, maxdB, mindB, maxPos, minPos)));
    }

    void drawLevel(juce::Graphics& g, float level, int x, int width);
    void updateLevel(float newLevel, float& smoothedLevel, float& leveldB) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeter)
};
