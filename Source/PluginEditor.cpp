#include "PluginEditor.h"
#include "PluginProcessor.h"
#include <BinaryData.h>

static juce::String formatTime (double ms)
{
    if (ms >= 1000.0)
        return juce::String (ms / 1000.0, 2) + " s";
    
    return juce::String (ms, 1) + " ms";
}

static juce::String formatFreq (double hz)
{
    if (hz >= 1000.0)
        return juce::String (hz / 1000.0, 2) + " kHz";
    
    return juce::String (hz, 2) + " Hz";
}

BPM2HzAudioProcessorEditor::BPM2HzAudioProcessorEditor (BPM2HzAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setLookAndFeel (&customLNF);
    setSize (760, 520);

    addAndMakeVisible (syncButton);
    syncButton.setButtonText ("DAW SYNC");

    addAndMakeVisible (bpmSlider);
    bpmSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    bpmSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 75, 20);

    syncAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "sync", syncButton);

    bpmAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "manualBpm", bpmSlider);

    startTimerHz (30);
}

BPM2HzAudioProcessorEditor::~BPM2HzAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

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
    // Posizionamento della levetta analogica e del knob Distressor
    syncButton.setBounds (480, 48, 120, 36);
    bpmSlider.setBounds (615, 12, 120, 110);
}

void BPM2HzAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto width = static_cast<float> (getWidth());
    auto height = static_cast<float> (getHeight());

    // -------------------------------------------------------------
    // 1. EFFETTO METALLO NERO SPAZZOLATO (Brushed Black Metal)
    // -------------------------------------------------------------
    juce::ColourGradient metalGradient (
        juce::Colour (0xff1c1f24), 0.0f, 0.0f,
        juce::Colour (0xff111317), width, height, false);
    metalGradient.addColour (0.4, juce::Colour (0xff242830));
    metalGradient.addColour (0.7, juce::Colour (0xff15171c));
    
    g.setGradientFill (metalGradient);
    g.fillAll();

    // Micro-scanalature e graffi metallici orizzontali
    juce::Random rng (12345);
    for (int y = 0; y < getHeight(); y += 2)
    {
        float alpha = rng.nextFloat() * 0.04f;
        if (rng.nextBool())
            g.setColour (juce::Colours::white.withAlpha (alpha));
        else
            g.setColour (juce::Colours::black.withAlpha (alpha * 1.5f));

        g.drawHorizontalLine (y, 0.0f, width);
    }

    // Vignettatura d'angolo e Bordo di finitura
    juce::ColourGradient vignette (
        juce::Colours::transparentBlack, width * 0.5f, height * 0.5f,
        juce::Colour (0xaa000000), 0.0f, 0.0f, true);
    g.setGradientFill (vignette);
    g.fillAll();

    g.setColour (juce::Colour (0x33ffffff));
    g.drawRect (0.0f, 0.0f, width, height, 1.0f);

    // -------------------------------------------------------------
    // 2. HEADER PANEL (Logo e Branding)
    // -------------------------------------------------------------
    juce::Rectangle<float> headerBox (15.0f, 12.0f, 730.0f, 110.0f);
    
    g.setColour (juce::Colour (0xd90f1115));
    g.fillRoundedRectangle (headerBox, 8.0f);
    
    g.setColour (juce::Colour (0x22ffffff));
    g.drawRoundedRectangle (headerBox, 8.0f, 1.0f);

    // Glow circolare azzurro dietro al logo DÆBÆR
    g.setColour (juce::Colour (0x1800e5ff));
    g.fillEllipse (20.0f, 15.0f, 100.0f, 100.0f);

    auto logoImage = juce::ImageCache::getFromMemory (BinaryData::daebaer_logo_png, BinaryData::daebaer_logo_pngSize);
    g.drawImageWithin (logoImage, 25, 20, 90, 90, juce::RectanglePlacement::centred);

    // Testo sotto il logo: solo "PLUGINS"
    g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
    g.setColour (juce::Colour (0xff00e5ff));
    g.drawText ("PLUGINS", 130, 48, 150, 20, juce::Justification::left);

    g.setFont (juce::FontOptions (14.0f, juce::Font::plain));
    g.setColour (juce::Colour (0xff8a95a5));
    g.drawText ("BPM2Hz Converter", 130, 68, 200, 22, juce::Justification::left);

    // -------------------------------------------------------------
    // 3. TABELLA VALORI (Header Colonne)
    // -------------------------------------------------------------
    int startX = 20;
    int startY = 140;
    int colWidth = 110;
    int rowHeight = 42;

    g.setFont (juce::FontOptions (12.0f, juce::Font::bold));

    g.setColour (juce::Colour (0xff8a95a5));
    g.drawText ("DIV", startX, startY, 60, rowHeight, juce::Justification::centred);

    struct HeaderDef { juce::String title; juce::Colour color; int colIdx; };
    std::vector<HeaderDef> headers = {
        { "NORM (TIME)",   juce::Colour (0xff00e676), 0 },
        { "NORM (FREQ)",   juce::Colour (0xff00e676), 1 },
        { "DOTTED (TIME)", juce::Colour (0xffff9100), 2 },
        { "DOTTED (FREQ)", juce::Colour (0xffff9100), 3 },
        { "TRIPLET (TIME)",juce::Colour (0xff00e5ff), 4 },
        { "TRIPLET (FREQ)",juce::Colour (0xff00e5ff), 5 }
    };

    for (const auto& h : headers)
    {
        int xPos = startX + 70 + (colWidth * h.colIdx);
        g.setColour (h.color);
        g.drawText (h.title, xPos, startY, colWidth, rowHeight, juce::Justification::centred);
    }

    g.setColour (juce::Colour (0x30ffffff));
    g.drawLine (static_cast<float>(startX), static_cast<float>(startY + rowHeight - 2),
                static_cast<float>(startX + 720), static_cast<float>(startY + rowHeight - 2), 1.0f);

    // -------------------------------------------------------------
    // 4. RIGHE DELLA TABELLA
    // -------------------------------------------------------------
    g.setFont (juce::FontOptions (13.0f, juce::Font::plain));
    int currentY = startY + rowHeight;

    for (size_t i = 0; i < processorRef.noteTable.size(); ++i)
    {
        const auto& item = processorRef.noteTable[i];

        if (i % 2 == 0)
        {
            g.setColour (juce::Colour (0x15ffffff));
            g.fillRoundedRectangle (static_cast<float>(startX), static_cast<float>(currentY), 720.0f, static_cast<float>(rowHeight - 4), 5.0f);
        }

        g.setColour (juce::Colour (0xfff0f0f0));
        g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
        g.drawText (item.name, startX, currentY, 60, rowHeight - 4, juce::Justification::centred);

        g.setFont (juce::FontOptions (13.0f, juce::Font::plain));

        // Valori NORM
        g.setColour (juce::Colour (0xffa3e9a4));
        g.drawText (formatTime (item.msNormal), startX + 70, currentY, colWidth, rowHeight - 4, juce::Justification::centred);
        g.drawText (formatFreq (item.hzNormal), startX + 70 + colWidth, currentY, colWidth, rowHeight - 4, juce::Justification::centred);

        // Valori DOTTED
        g.setColour (juce::Colour (0xffffcc80));
        g.drawText (formatTime (item.msDotted), startX + 70 + colWidth * 2, currentY, colWidth, rowHeight - 4, juce::Justification::centred);
        g.drawText (formatFreq (item.hzDotted), startX + 70 + colWidth * 3, currentY, colWidth, rowHeight - 4, juce::Justification::centred);

        // Valori TRIPLET
        g.setColour (juce::Colour (0xff80deea));
        g.drawText (formatTime (item.msTriplet), startX + 70 + colWidth * 4, currentY, colWidth, rowHeight - 4, juce::Justification::centred);
        g.drawText (formatFreq (item.hzTriplet), startX + 70 + colWidth * 5, currentY, colWidth, rowHeight - 4, juce::Justification::centred);

        currentY += rowHeight;
    }
}