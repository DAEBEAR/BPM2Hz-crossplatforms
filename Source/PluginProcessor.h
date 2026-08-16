#pragma once
#include <JuceHeader.h>

struct NoteTime
{
    juce::String name;
    float multiplier;
    float msNormal, hzNormal;
    float msDotted, hzDotted;
    float msTriplet, hzTriplet;
};

class BPM2HzAudioProcessor : public juce::AudioProcessor
{
public:
    BPM2HzAudioProcessor();
    ~BPM2HzAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "BPM2Hz"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override {}
    void setStateInformation (const void*, int) override {}

    float currentBpm { 120.0f };
    std::vector<NoteTime> noteTable;

private:
    void updateTable();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BPM2HzAudioProcessor)
};