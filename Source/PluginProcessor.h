#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

struct NoteTime
{
    juce::String name;
    float multiplier;
    float msNormal { 0.0f };
    float hzNormal { 0.0f };
    float msDotted { 0.0f };
    float hzDotted { 0.0f };
    float msTriplet { 0.0f };
    float hzTriplet { 0.0f };
};

class BPM2HzAudioProcessor  : public juce::AudioProcessor
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
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void updateTable();

    std::vector<NoteTime> noteTable;
    float currentBpm { 120.0f };

    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BPM2HzAudioProcessor)
};