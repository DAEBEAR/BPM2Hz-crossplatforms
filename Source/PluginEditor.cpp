#include "PluginEditor.h"
#include "PluginProcessor.h"
#include <BinaryData.h>

// Helper per la formattazione del tempo (ms o s)
static juce::String formatTime (double ms)
{
    if (ms >= 1000.0)
        return juce::String (ms / 1000.0, 2) + " s";
    
    return juce::String (ms, 1) + " ms";
}

// Helper per la formattazione della frequenza (Hz o kHz)
static juce::String formatFreq (double hz)
{
    if (hz >= 1000.0)
        return juce::String (hz / 1000.0, 2) + " kHz";
    
    return juce::String (hz, 2) + " Hz";
}

BPM2HzAudioProcessorEditor::BPM2HzAudioProcessorEditor (BPM2HzAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setSize (750, 510);

    addAndMakeVisible (syncButton);
    syncButton.setButtonText ("DAW SYNC");
    syncButton.setColour (juce::ToggleButton::textColourId, juce::Colours::white);
    syncButton.setColour (juce::ToggleButton::tickColourId, juce::Colours::cyan);

    addAndMakeVisible (bpmSlider);
    bpmSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    bpmSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    bpmSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::cyan);
    bpmSlider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    bpmSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colours::cyan);

    syncAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "sync", syncButton);

    bpmAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "manualBpm", bpmSlider);

    startTimerHz (30);
}

BPM2HzAudioProcessorEditor::~BPM2HzAudioProcessorEditor() {}

void BPM2HzAudioProcessorEditor::timerCallback()
{
    bool isSynced = processorRef.apvts.getRawParameterValue ("sync")->load() > 0.5f;

    if (isSynced)
    {
        bpmSlider.setEnabled (false);
        bpmSlider.setValue (processorRef.currentBpm, juce::dontSendNotification);
    }
    else
    {
        bpmSlider.setEnabled (true);
    }

    repaint();
}

void BPM2HzAudioProcessorEditor::resized()
{
    syncButton.setBounds (500, 35, 100, 30);
    bpmSlider.setBounds (610, 10, 110, 100);
}

void BPM2HzAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff121212));

    auto logoImage = juce::ImageCache::getFromMemory (BinaryData::daebaer_logo_png, BinaryData::daebaer_logo_pngSize);
    g.drawImageWithin (logoImage, 20, 15, 110, 110, juce::RectanglePlacement::centred);

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (22.0f, juce::Font::bold));
    g.drawText (juce::String::fromUTF8 (u8"DÆBÆR PLUGINS"), 140, 30, 300, 30, juce::Justification::left);

    g.setFont (juce::FontOptions (15.0f, juce::Font::plain));
    g.setColour (juce::Colour (0xffaaaaaa));
    g.drawText ("BPM2Hz Converter", 140, 58, 300, 25, juce::Justification::left);

    int startX = 20;
    int startY = 140;
    int colWidth = 110;
    int rowHeight = 40;

    g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
    g.setColour (juce::Colour (0xff888888));

    g.drawText ("DIV", startX, startY, 60, rowHeight, juce::Justification::centred);
    g.drawText ("NORM (TIME)", startX + 70, startY, colWidth, rowHeight, juce::Justification::centred);
    g.drawText ("NORM (FREQ)", startX + 70 + colWidth, startY, colWidth, rowHeight, juce::Justification::centred);
    g.drawText ("DOTTED (TIME)", startX + 70 + colWidth * 2, startY, colWidth, rowHeight, juce::Justification::centred);
    g.drawText ("DOTTED (FREQ)", startX + 70 + colWidth * 3, startY, colWidth, rowHeight, juce::Justification::centred);
    g.drawText ("TRIPLET (TIME)", startX + 70 + colWidth * 4, startY, colWidth, rowHeight, juce::Justification::centred);
    g.drawText ("TRIPLET (FREQ)", startX + 70 + colWidth * 5, startY, colWidth, rowHeight, juce::Justification::centred);

    g.setFont (juce::FontOptions (13.0f, juce::Font::plain));
    int currentY = startY + rowHeight;

    for (size_t i = 0; i < processorRef.noteTable.size(); ++i)
    {
        const auto& item = processorRef.noteTable[i];

        if (i % 2 == 0)
        {
            g.setColour (juce::Colour (0xff1a1a1a));
            g.fillRoundedRectangle (static_cast<float>(startX), static_cast<float>(currentY), 710.0f, static_cast<float>(rowHeight - 4), 4.0f);
        }

        g.setColour (juce::Colours::white);
        g.drawText (item.name, startX, currentY, 60, rowHeight - 4, juce::Justification::centred);

        // NORM
        g.setColour (juce::Colours::lightgreen);
        g.drawText (formatTime (item.msNormal), startX + 70, currentY, colWidth, rowHeight - 4, juce::Justification::centred);
        g.drawText (formatFreq (item.hzNormal), startX + 70 + colWidth, currentY, colWidth, rowHeight - 4, juce::Justification::centred);

        // DOTTED
        g.setColour (juce::Colours::orange);
        g.drawText (formatTime (item.msDotted), startX + 70 + colWidth * 2, currentY, colWidth, rowHeight - 4, juce::Justification::centred);
        g.drawText (formatFreq (item.hzDotted), startX + 70 + colWidth * 3, currentY, colWidth, rowHeight - 4, juce::Justification::centred);

        // TRIPLET
        g.setColour (juce::Colours::cyan);
        g.drawText (formatTime (item.msTriplet), startX + 70 + colWidth * 4, currentY, colWidth, rowHeight - 4, juce::Justification::centred);
        g.drawText (formatFreq (item.hzTriplet), startX + 70 + colWidth * 5, currentY, colWidth, rowHeight - 4, juce::Justification::centred);

        currentY += rowHeight;
    }
}