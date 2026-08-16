void BPM2HzAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff121212));

    auto logoImage = juce::ImageCache::getFromMemory (BinaryData::daebaer_logo_png, BinaryData::daebaer_logo_pngSize);
    g.drawImageWithin (logoImage, 20, 15, 110, 110, juce::RectanglePlacement::centred);

    // Utilizzo del prefisso u8 per gestire correttamente i caratteri UTF-8 (Æ)
    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (22.0f, juce::Font::bold));
    g.drawText (juce::String (juce::CharPointer_UTF8 (u8"DÆBÆR PLUGINS")), 140, 30, 300, 30, juce::Justification::left);

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
            g.fillRoundedRectangle (static_cast<float>(startX), static_cast<float>(currentY), 710.0f, static_cast<float>(rowHeight - 4), 4.0f);
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