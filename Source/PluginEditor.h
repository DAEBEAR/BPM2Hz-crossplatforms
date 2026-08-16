#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class BPM2HzAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    BPM2HzAudioProcessorEditor (BPM2HzAudioProcessor&);
    ~BPM2HzAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override {}

private:
    void timerCallback() override;

    BPM2HzAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BPM2HzAudioProcessorEditor)
};