/*
  ==============================================================================

    LookAndFeel.h
    Created: 21 Feb 2025 9:45:38am
    Author:  kyleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace Colors
{
    const juce::Colour background{ 0, 0, 0 }; // Black background
    const juce::Colour header{ 50, 50, 50 }; // Dark gray for headers

    namespace Knob
    {
        const juce::Colour trackBackground{ 60, 60, 60 }; // Dim gray
        const juce::Colour trackActive{ 255, 100, 0 }; // Electric tangerine
        const juce::Colour outline{ 180, 70, 190 }; // Vibrant magenta-plum
        const juce::Colour gradientTop{ 255, 130, 50 }; // Bright tangerine
        const juce::Colour gradientBottom{ 245, 100, 45 }; // More glow, more balanced
        const juce::Colour dial{ 200, 90, 255 }; // Glowing violet
        const juce::Colour dropShadow{ 25, 25, 25 }; // Soft dark shadow
        const juce::Colour label{ 220, 220, 220 }; // Light gray
        const juce::Colour textBoxBackground{ 45, 45, 45 }; // Slightly darker gray
        const juce::Colour value{ 255, 255, 255 }; // White
        const juce::Colour caret{ 255, 100, 0 }; // Tangerine caret
    }

    namespace Group
    {
        const juce::Colour label{ 230, 120, 50 }; // Updated darker orange
        const juce::Colour outline{ 102, 51, 153 }; // Royal plum outline
    }

    namespace Button
    {
        const juce::Colour text{ 230, 120, 50 }; // Updated deeper orange
        const juce::Colour textToggled{ 255, 255, 255 }; // White when toggled
        const juce::Colour background{ 50, 50, 50 }; // Dark gray
        const juce::Colour backgroundToggled{ 102, 51, 153 }; // Royal plum when toggled
        const juce::Colour outline{ 230, 120, 50 }; // Updated orange outline
    }

    namespace LevelMeter
    {
        const juce::Colour background{ 20, 20, 20 }; // Very dark gray
        const juce::Colour tickLine{ 200, 200, 200 }; // Light gray tick marks
        const juce::Colour tickLabel{ 230, 120, 50 }; // Updated orange labels
        const juce::Colour tooLoud{ 226, 74, 81 }; // Red for overload
        const juce::Colour levelOK{ 65, 206, 88 }; // Green for safe levels
    }

    namespace Slider
    {
        const juce::Colour sliderFill{ 160, 70, 220 };
        const juce::Colour standoutFill{ 50, 200, 180 }; // Cyan for special slider
    }

}

class Fonts
{
public:
    Fonts() = delete;
    static juce::Font getFont(float height = 16.f);

private:
    static const juce::Typeface::Ptr typeface;
};

class RotaryKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:

    RotaryKnobLookAndFeel();

    static RotaryKnobLookAndFeel* get()
    {
        static RotaryKnobLookAndFeel instance;
        return &instance;
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider& slider) override;

    juce::Font getLabelFont(juce::Label&) override;

    juce::Label* createSliderTextBox(juce::Slider&) override;

    void drawTextEditorOutline(juce::Graphics&, int, int, juce::TextEditor&) override
    {
    }

    void fillTextEditorBackground(juce::Graphics&, int width, int height, juce::TextEditor&) override;

private:

    juce::DropShadow dropShadow{ Colors::Knob::dropShadow, 6, {0, 3} };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryKnobLookAndFeel)
};

class MainLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MainLookAndFeel();

    juce::Font getLabelFont(juce::Label&) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainLookAndFeel)
};


class ButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ButtonLookAndFeel();

    static ButtonLookAndFeel* get()
    {
        static ButtonLookAndFeel instance;
        return &instance;
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
        const juce::Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonLookAndFeel)
};