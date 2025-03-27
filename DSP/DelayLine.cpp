/*
  ==============================================================================

    DelayLine.cpp
    Created: 5 Mar 2025 8:28:35am
    Author:  kyleb

  ==============================================================================
*/

#include <JuceHeader.h>
#include "../DSP/DelayLine.h"

//==============================================================================
void DelayLine::setMaximumDelayInSamples(int maxLengthInSamples)
{
    jassert(maxLengthInSamples > 0);

    const int paddedLength = maxLengthInSamples + 2;

    if (bufferLength < paddedLength)
    {
        bufferLength = paddedLength;
        buffer.reset(new float[size_t(bufferLength)]);
    }
}

void DelayLine::reset()
{
    writeIndex = bufferLength - 1;

    for (size_t i = 0; i < size_t(bufferLength); ++i)
        buffer[i] = 0.f;
}

void DelayLine::write(float input) noexcept
{
    jassert(bufferLength > 0);

    if (++writeIndex >= bufferLength)
        writeIndex = 0;

    buffer[size_t(writeIndex)] = input;
}

float DelayLine::read(float delayInSamples) const noexcept
{
    jassert(delayInSamples >= 1.f);
    jassert(delayInSamples <= bufferLength - 2.f);

    const int integerDelay = int(delayInSamples);

    int readIndexA = writeIndex - integerDelay + 1;
    int readIndexB = readIndexA - 1;
    int readIndexC = readIndexA - 2;
    int readIndexD = readIndexA - 3;

    if (readIndexD < 0)
    {
        readIndexD += bufferLength;
        if (readIndexC < 0)
        {
            readIndexC += bufferLength;
            if (readIndexB < 0)
            {
                readIndexB += bufferLength;
                if (readIndexA < 0)
                    readIndexA += bufferLength;
            }
        }
    }

    const float sampleA = buffer[size_t(readIndexA)];
    const float sampleB = buffer[size_t(readIndexB)];
    const float sampleC = buffer[size_t(readIndexC)];
    const float sampleD = buffer[size_t(readIndexD)];

    const float fraction = delayInSamples - float(integerDelay);

    const float slope0 = 0.5f * (sampleC - sampleA);
    const float slope1 = 0.5f * (sampleD - sampleB);
    const float v = sampleB - sampleC;
    const float w = slope0 + v;

    const float a = w + v + slope1;
    const float b = w + a;
    const float stage1 = a * fraction - b;
    const float stage2 = stage1 * fraction + slope0;

    return stage2 * fraction + sampleB;
}
