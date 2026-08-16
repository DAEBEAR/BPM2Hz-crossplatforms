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

    startTimerHz (20);
}

BPM2HzAudioProcessorEditor::~BPM2HzAudioProcessorEditor()
{
    stopTimer();
    syncAttachment = nullptr;
    bpmAttachment = nullptr;
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

    if (isShowing())
        repaint();
}

void BPM2HzAudioProcessorEditor::resized()
{
    syncButton.setBounds (480, 48, 120, 36);
    bpmSlider.setBounds (615, 12, 120, 110);
}

void BPM2HzAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto width = static_cast<float> (getWidth());
    auto height = static_cast<float> (getHeight());

    // -------------------------------------------------------------
    // 1. SFONDO REALE DA IMMAGINE (BRUSHED METAL)
    // -------------------------------------------------------------
    int bgSize = 0;
    const char* bgData = BinaryData::getNamedResource ("brushed_metal_jpg", bgSize);
    if (bgData == nullptr) bgData = BinaryData::getNamedResource ("brushed_metal.jpg", bgSize);

    auto bgMetal = (bgData != nullptr && bgSize > 0)
                        ? juce::ImageCache::getFromMemory (bgData, bgSize)
                        : juce::Image();
    
    if (bgMetal.isValid())
        g.drawImage (bgMetal, getLocalBounds().toFloat(), juce::RectanglePlacement::fillDestination);
    else
        g.fillAll (juce::Colour (0xff121417));

    // Vignettatura morbida
    juce::ColourGradient vignette (
        juce::Colours::transparentBlack, width * 0.5f, height * 0.5f,
        juce::Colour (0xbb000000), 0.0f, 0.0f, true);
    g.setGradientFill (vignette);
    g.fillAll();

    // Bordo esterno plugin
    g.setColour (juce::Colour (0x33ffffff));
    g.drawRect (0.0f, 0.0f, width, height, 1.0f);

    // -------------------------------------------------------------
    // 2. HEADER PANEL
    // -------------------------------------------------------------
    juce::Rectangle<float> headerBox (15.0f, 12.0f, 730.0f, 110.0f);
    g.setColour (juce::Colour (0xcc0b0d10));
    g.fillRoundedRectangle (headerBox, 8.0f);
    g.setColour (juce::Colour (0x22ffffff));
    g.drawRoundedRectangle (headerBox, 8.0f, 1.0f);

    // LOGO DÆBÆR
    int logoSize = 0;
    const char* logoData = BinaryData::getNamedResource ("daebaer_logo_png", logoSize);
    if (logoData == nullptr) logoData = BinaryData::getNamedResource ("daebaer_logo.png", logoSize);

    auto logoImage = (logoData != nullptr && logoSize > 0)
                        ? juce::ImageCache::getFromMemory (logoData, logoSize)
                        : juce::Image();
                        
    if (logoImage.isValid())
    {
        // Logo centrato verticalmente a sinistra
        g.drawImageWithin (logoImage, 25, 18, 75, 75, juce::RectanglePlacement::centred);
    }

    // SCRITTA "PLUGINS" SOTTO IL LOGO IN BIANCO TRASPARENTE
    g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
    g.setColour (juce::Colour (0x88ffffff)); // Bianco trasparente stile logo
    g.drawText ("PLUGINS", 20, 93, 85, 15, juce::Justification::centred);

    // NOME PLUGIN AFFIANCO
    g.setFont (juce::FontOptions (16.0f, juce::Font::bold));
    g.setColour (juce::Colour (0xffe0e0e0));
    g.drawText ("BPM2Hz Converter", 125, 48, 220, 24, juce::Justification::left);

    // -------------------------------------------------------------
    // 3. PANNELLO TABELLA SEMI-TRASPARENTE
    // -------------------------------------------------------------
    juce::Rectangle<float> tableBox (15.0f, 132.0f, 730.0f, 375.0f);
    g.setColour (juce::Colour (0xcc0b0d10));
    g.fillRoundedRectangle (tableBox, 8.0f);
    g.setColour (juce::Colour (0x22ffffff));
    g.drawRoundedRectangle (tableBox, 8.0f, 1.0f);

    // -------------------------------------------------------------
    // 4. TABELLA VALORI (Header Colonne in Scala di Grigi)
    // -------------------------------------------------------------
    int startX = 20;
    int startY = 140;
    int colWidth = 110;
    int rowHeight = 42;

    g.setFont (juce::FontOptions (11.0f, juce::Font::bold));

    g.setColour (juce::Colour (0xff8a95a5)); // Grigio chiaro
    g.drawText ("DIV", startX, startY, 60, rowHeight, juce::Justification::centred);

    juce::String headers[6] = {
        "NORM (TIME)", "NORM (FREQ)",
        "DOTTED (TIME)", "DOTTED (FREQ)",
        "TRIPLET (TIME)", "TRIPLET (FREQ)"
    };

    for (int col = 0; col < 6; ++col)
    {
        int xPos = startX + 70 + (colWidth * col);
        g.setColour (juce::Colour (0xffb0b8c4)); // Scala di grigi leggibile
        g.drawText (headers[col], xPos, startY, colWidth, rowHeight, juce::Justification::centred);
    }

    // Linea Orizzontale Separatore Header
    g.setColour (juce::Colour (0x30ffffff));
    g.drawLine (static_cast<float>(startX), static_cast<float>(startY + rowHeight - 2),
                static_cast<float>(startX + 720), static_cast<float>(startY + rowHeight - 2), 1.0f);

    // -------------------------------------------------------------
    // 5. RIGHE DELLA TABELLA E LINEE VERTICALI
    // -------------------------------------------------------------
    int currentY = startY + rowHeight;
    const size_t numRows = processorRef.noteTable.size();

    for (size_t i = 0; i < numRows; ++i)
    {
        const auto item = processorRef.noteTable[i];

        // Sfondo riga alternato
        if (i % 2 == 0)
        {
            g.setColour (juce::Colour (0x12ffffff));
            g.fillRoundedRectangle (static_cast<float>(startX), static_cast<float>(currentY), 720.0f, static_cast<float>(rowHeight - 4), 4.0f);
        }

        // Nome Divisione (Bianco Nitido)
        g.setColour (juce::Colour (0xffffffff));
        g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
        g.drawText (item.name, startX, currentY, 60, rowHeight - 4, juce::Justification::centred);

        // Valori in Scala di Grigi (Alto Contrasto)
        g.setFont (juce::FontOptions (13.0f, juce::Font::plain));
        g.setColour (juce::Colour (0xffd1d5db)); // Grigio bilanciato ad alta leggibilità

        g.drawText (formatTime (item.msNormal),  startX + 70,                currentY, colWidth, rowHeight - 4, juce::Justification::centred);
        g.drawText (formatFreq (item.hzNormal),  startX + 70 + colWidth,     currentY, colWidth, rowHeight - 4, juce::Justification::centred);
        g.drawText (formatTime (item.msDotted),  startX + 70 + colWidth * 2, currentY, colWidth, rowHeight - 4, juce::Justification::centred);
        g.drawText (formatFreq (item.hzDotted),  startX + 70 + colWidth * 3, currentY, colWidth, rowHeight - 4, juce::Justification::centred);
        g.drawText (formatTime (item.msTriplet), startX + 70 + colWidth * 4, currentY, colWidth, rowHeight - 4, juce::Justification::centred);
        g.drawText (formatFreq (item.hzTriplet), startX + 70 + colWidth * 5, currentY, colWidth, rowHeight - 4, juce::Justification::centred);

        currentY += rowHeight;
    }

    // -------------------------------------------------------------
    // 6. LINEE VERTICALI DI SEPARAZIONE TABELLA
    // -------------------------------------------------------------
    g.setColour (juce::Colour (0x1affffff)); // Grigio sfumato semi-trasparente

    // Linea tra "DIV" e i valori
    g.drawLine (static_cast<float>(startX + 65), static_cast<float>(startY + 6),
                static_cast<float>(startX + 65), static_cast<float>(startY + 360), 1.0f);

    // Linee tra ogni colonna di dati
    for (int col = 1; col < 6; ++col)
    {
        int xLine = startX + 70 + (colWidth * col);
        g.drawLine (static_cast<float>(xLine), static_cast<float>(startY + 6),
                    static_cast<float>(xLine), static_cast<float>(startY + 360), 1.0f);
    }
}