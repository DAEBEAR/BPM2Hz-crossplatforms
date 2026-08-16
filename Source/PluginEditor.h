#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include <BinaryData.h>

// LookAndFeel per utilizzare l'immagine knob.png dagli assets e la Levetta Analogica
class DistressorLNF : public juce::LookAndFeel_V4
{
public:
    DistressorLNF()
    {
        setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xfff0f0f0));
    }

    // Disegna il knob usando l'immagine originale knob.png dagli assets
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (4.0f);
        auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto center = bounds.getCentre();
        
        // Angolo corrente calcolato in base alla posizione dello slider
        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // Caricamento dell'immagine del knob dagli assets (knob.png)
        auto knobImage = juce::ImageCache::getFromMemory (BinaryData::knob_png, BinaryData::knob_pngSize);

        if (knobImage.isValid())
        {
            juce::Graphics::ScopedSaveState saveState (g);

            // Applica la rotazione attorno al centro della manopola
            g.addTransform (juce::AffineTransform::rotation (angle, center.x, center.y));

            // Disegna l'immagine del knob centrandola nell'area riservata
            auto destRect = juce::Rectangle<float> (center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);
            g.drawImage (knobImage, destRect, juce::RectanglePlacement::centred);
        }
        else
        {
            // Disegno di riserva se l'asset knob.png non viene trovato
            g.setColour (juce::Colours::darkgrey);
            g.fillEllipse (bounds);
        }
    }

    // Design Levetta Analogica (Toggle Switch)
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