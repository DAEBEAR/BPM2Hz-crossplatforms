#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

struct NoteValueVal
{
    juce::String name;
    double msNormal { 0.0 };
    double hzNormal { 0.0 };
    double msDotted { 0.0 };
    double hzDotted { 0.0 };
    double msTriplet { 0.0 };
    double hzTriplet { 0.0 };
};

class BPM2HzAudioProcessor  : public juce::AudioProcessor
{
public:
    BPM2HzAudioProcessor();
    ~BPM2HzAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void updateBpmTable (float bpm);

    std::vector<NoteValueVal> noteTable;
    float currentBpm { 120.0f };

    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BPM2HzAudioProcessor)
};