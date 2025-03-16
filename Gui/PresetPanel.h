/*
  ==============================================================================

    PresetPanel.h
    Created: 13 Mar 2025 9:17:02am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace Gui
{
    class PresetPanel : public juce::Component, juce::Button::Listener, juce::ComboBox::Listener
    {
    public:
        PresetPanel()
        {
            configureButton(saveButton, "Save");
            configureButton(deleteButton, "Delete");
            configureButton(previousPresetButton, "<");
            configureButton(nextPresetButton, ">");

            presetList.setTextWhenNothingSelected("No Preset Selected");
            presetList.setMouseCursor(juce::MouseCursor::PointingHandCursor);
            addAndMakeVisible(presetList);
            presetList.addListener(this);
        }

        ~PresetPanel()
        {
            saveButton.removeListener(this);
            deleteButton.removeListener(this);
            previousPresetButton.removeListener(this);
            nextPresetButton.removeListener(this);
            presetList.removeListener(this);
        }

        void resized() override
        {
            const auto container = getLocalBounds().reduced(4);
            auto bounds = container;

            saveButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.2f)).reduced(4));
            previousPresetButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(4));
            presetList.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.4f)).reduced(4));
            nextPresetButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(4));
            deleteButton.setBounds(bounds.reduced(4));
        }

    private:
        juce::TextButton saveButton, deleteButton, previousPresetButton, nextPresetButton;
        juce::ComboBox presetList;

        void buttonClicked(juce::Button*) override
        {
            return;
        }
        void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override
        {
            return;
        }

        void configureButton(juce::Button& button, const juce::String& buttontext)
        {
            button.setButtonText(buttontext);
            button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
            addAndMakeVisible(button);
            button.addListener(this);

        }


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetPanel)

    };
}
