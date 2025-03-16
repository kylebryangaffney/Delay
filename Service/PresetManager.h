/*
  ==============================================================================

    PresetManager.h
    Created: 16 Mar 2025 10:12:14am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace Service
{
    class PresetManager
    {
    public:
        static const juce::File defaultDirectory;
        static const juce::String extension;

        PresetManager(juce::AudioProcessorValueTreeState& apvts);
        void savePreset(const juce::String& presetName);
        void deletePreset(const juce::String& presetName);
        void loadPreset(const juce::String& presetName);
        void loadNextPreset();
        void loadPreviousPreset();
        juce::StringArray getAllPresets() const;
        juce::String getCurrentPreset() const;
    private:
        juce::AudioProcessorValueTreeState& apvts;
        juce::String currentPreset;

    };
}