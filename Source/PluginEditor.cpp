#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <BinaryData.h>

BPM2HzAudioProcessorEditor::BPM2HzAudioProcessorEditor (BPM2HzAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setSize (750, 480);
    startTimerHz (30);
}

BPM2HzAudioProcessorEditor::~BPM2HzAudioProcessorEditor() {}

void BPM2HzAudioProcessorEditor::timerCallback()
{
    repaint();
}

void BPM2HzAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff121212));

    auto logoImage = juce::ImageCache::getFromMemory (BinaryData::DAEBAER_Logo_png, BinaryData::DAEBAER_Logo_pngSize);
    g.drawImageWithin (logoImage, 20, 20, 120, 120, juce::RectanglePlacement::centred);

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (22.0f, juce::Font::bold));
    g.drawText ("DÆBÆR PLUGINS", 160, 25, 300, 30, juce::Justification::left);

    g.setFont (juce::FontOptions (16.0f, juce::Font::plain));
    g.setColour (juce::Colour (0xffaaaaaa));
    g.drawText ("BPM2Hz Converter", 160, 50, 300, 25, juce::Justification::left);

    g.setColour (juce::Colour (0xff252525));
    g.fillRoundedRectangle (550, 20, 170, 60, 8.0f);
    g.setColour (juce::Colours::cyan);
    g.setFont (juce::FontOptions (24.0f, juce::Font::bold));
    g.drawText (juce::String (processorRef.currentBpm, 1) + " BPM", 550, 20, 170, 60, juce::Justification::centred);

    int startX = 20;
    int startY = 130;
    int colWidth = 110;
    int rowHeight = 40;

    g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
    g.setColour (juce::Colour (0xff888888));

    g.drawText ("DIV", startX, startY, 60, rowHeight, juce::Justification::centred);
    g.drawText ("NORM (ms)", startX + 70, startY, colWidth, rowHeight, juce::Justification::centred);
    g.drawText ("NORM (Hz)", startX + 70 + colWidth, startY, colWidth, rowHeight, juce::Justification::centred);
    g.drawText ("DOTTED (ms)", startX + 70 + colWidth * 2, startY, colWidth, rowHeight, juce::Justification::centred);
    g.drawText ("DOTTED (Hz)", startX + 70 + colWidth * 3, startY, colWidth, rowHeight, juce::Justification::centred);
    g.drawText ("TRIPLET (ms)", startX + 70 + colWidth * 4, startY, colWidth, rowHeight, juce::Justification::centred);
    g.drawText ("TRIPLET (Hz)", startX + 70 + colWidth * 5, startY, colWidth, rowHeight, juce::Justification::centred);

    g.setFont (juce::FontOptions (13.0f, juce::Font::plain));
    int currentY = startY + rowHeight;

    for (size_t i = 0; i < processorRef.noteTable.size(); ++i)
    {
        const auto& item = processorRef.noteTable[i];

        if (i % 2 == 0)
        {
            g.setColour (juce::Colour (0xff1a1a1a));
            g.fillRoundedRectangle (startX, currentY, 710, rowHeight - 4, 4.0f);
        }

        g.setColour (juce::Colours::white);
        g.drawText (item.name, startX, currentY, 60, rowHeight - 4, juce::Justification::centred);

        g.setColour (juce::Colours::lightgreen);
        g.drawText (juce::String (item.msNormal, 1), startX + 70, currentY, colWidth, rowHeight - 4, juce::Justification::centred);
        g.drawText (juce::String (item.hzNormal, 2), startX + 70 + colWidth, currentY, colWidth, rowHeight - 4, juce::Justification::centred);

        g.setColour (juce::Colours::orange);
        g.drawText (juce::String (item.msDotted, 1), startX + 70 + colWidth * 2, currentY, colWidth, rowHeight - 4, juce::Justification::centred);
        g.drawText (juce::String (item.hzDotted, 2), startX + 70 + colWidth * 3, currentY, colWidth, rowHeight - 4, juce::Justification::centred);

        g.setColour (juce::Colours::cyan);
        g.drawText (juce::String (item.msTriplet, 1), startX + 70 + colWidth * 4, currentY, colWidth, rowHeight - 4, juce::Justification::centred);
        g.drawText (juce::String (item.hzTriplet, 2), startX + 70 + colWidth * 5, currentY, colWidth, rowHeight - 4, juce::Justification::centred);

        currentY += rowHeight;
    }
}