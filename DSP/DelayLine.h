/*
  ==============================================================================

    DelayLine.h
    Created: 5 Mar 2025 8:28:35am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <memory>

//==============================================================================
// A simple circular delay buffer with fractional sample support
class DelayLine
{
public:
    void setMaximumDelayInSamples(int maxLengthInSamples);
    void reset();

    void write(float input) noexcept;
    float read(float delayInSamples) const noexcept;

    int getBufferLength() const noexcept { return bufferLength; }

private:
    std::unique_ptr<float[]> buffer;
    int bufferLength = 0;
    int writeIndex = 0; // index of the most recently written sample
};
