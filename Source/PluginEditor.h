#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

// LookAndFeel personalizzato per Knob stile Distressor e Levetta Analogica
class DistressorLNF : public juce::LookAndFeel_V4
{
public:
    DistressorLNF()
    {
        setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xfff0f0f0));
    }

    // Design Knob stile Empirical Labs Distressor
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (12.0f);
        auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto center = bounds.getCentre();
        auto toAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // Tacche numerate ed esterne attorno alla manopola
        int numTicks = 11;
        g.setColour (juce::Colour (0xbbffffff));
        for (int i = 0; i < numTicks; ++i)
        {
            float angle = rotaryStartAngle + (i / static_cast<float>(numTicks - 1)) * (rotaryEndAngle - rotaryStartAngle);
            auto tickStart = center.getPointOnCircumference (radius + 2.0f, angle);
            auto tickEnd   = center.getPointOnCircumference (radius + 7.0f, angle);
            g.drawLine (tickStart.x, tickStart.y, tickEnd.x, tickEnd.y, (i % 2 == 0) ? 2.0f : 1.0f);
        }

        // Corpo Esterno della Manopola (Base Nera e Zigrinata)
        g.setColour (juce::Colour (0xff111215));
        g.fillEllipse (bounds);
        g.setColour (juce::Colour (0xff333742));
        g.drawEllipse (bounds, 2.5f);

        // Capsula Metallica Centrale
        auto innerBounds = bounds.reduced (radius * 0.35f);
        juce::ColourGradient capGradient (juce::Colour (0xff555d6b), innerBounds.getX(), innerBounds.getY(),
                                           juce::Colour (0xff1c2026), innerBounds.getRight(), innerBounds.getBottom(), false);
        g.setGradientFill (capGradient);
        g.fillEllipse (innerBounds);
        g.setColour (juce::Colour (0x44ffffff));
        g.drawEllipse (innerBounds, 1.0f);

        // Pointer Linea Bianca ad alto contrasto
        juce::Path pointer;
        auto pointerLength = radius - 3.0f;
        pointer.addRectangle (-2.5f, -pointerLength, 5.0f, pointerLength * 0.65f);
        pointer.applyTransform (juce::AffineTransform::rotation (toAngle).translated (center.x, center.y));

        g.setColour (slider.isEnabled() ? juce::Colours::white : juce::Colour (0xff666666));
        g.fillPath (pointer);
    }

    // Design Levetta Analogica (Toggle Switch Hardware)
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        auto switchArea = bounds.removeFromLeft (32.0f).reduced (2.0f);
        
        // Base incassata metallica dello switch
        g.setColour (juce::Colour (0xff0a0b0d));
        g.fillRoundedRectangle (switchArea, 4.0f);
        g.setColour (juce::Colour (0xff3a3f4d));
        g.drawRoundedRectangle (switchArea, 4.0f, 1.0f);

        // Levetta Fisica metallica (Up = ON, Down = OFF)
        bool isOn = button.getToggleState();
        auto leverY = isOn ? switchArea.getY() + 3.0f : switchArea.getBottom() - 15.0f;
        juce::Rectangle<float> lever (switchArea.getX() + 4.0f, leverY, switchArea.getWidth() - 8.0f, 12.0f);

        juce::ColourGradient leverGrad (juce::Colour (0xffe0e0e0), lever.getX(), lever.getY(),
                                        juce::Colour (0xff777777), lever.getRight(), lever.getBottom(), false);
        g.setGradientFill (leverGrad);
        g.fillRoundedRectangle (lever, 2.0f);

        // Testo Etichetta
        g.setColour (button.getToggleState() ? juce::Colour (0xff00e5ff) : juce::Colour (0xffaaaaaa));
        g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
        g.drawText (button.getButtonText(), bounds.reduced (6.0f, 0.0f), juce::Justification::centredLeft);
    }
};

class BPM2HzAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                    private juce::Timer
{
public:
    explicit BPM2HzAudioProcessorEditor (BPM2HzAudioProcessor&);
    ~BPM2HzAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    BPM2HzAudioProcessor& processorRef;

    juce::ToggleButton syncButton;
    juce::Slider bpmSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> syncAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bpmAttachment;

    DistressorLNF customLNF;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BPM2HzAudioProcessorEditor)
};