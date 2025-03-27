/*
  ==============================================================================

    Tempo.h
    Created: 3 Mar 2025 8:47:25am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class Tempo
{
public:
    Tempo() = default;

    void reset() noexcept;
    void update(const juce::AudioPlayHead* playhead) noexcept;

    double getMillisecondsForNoteLength(int index) const noexcept;
    double getTempo() const noexcept { return bpm; }

private:
    double bpm = 120.0;
};
