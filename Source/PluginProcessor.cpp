#include "PluginProcessor.h"
#include "PluginEditor.h"

BPM2HzAudioProcessor::BPM2HzAudioProcessor()
    : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    noteTable = {
        { "1 Bar", 4.0f },
        { "1/2",   2.0f },
        { "1/4",   1.0f },
        { "1/8",   0.5f },
        { "1/16",  0.25f },
        { "1/32",  0.125f },
        { "1/64",  0.0625f }
    };
    updateTable();
}

BPM2HzAudioProcessor::~BPM2HzAudioProcessor() {}

void BPM2HzAudioProcessor::prepareToPlay (double, int) {}
void BPM2HzAudioProcessor::releaseResources() {}

void BPM2HzAudioProcessor::updateTable()
{
    if (currentBpm <= 0.0f) return;

    float quarterMs = 60000.0f / currentBpm;

    for (auto& note : noteTable)
    {
        note.msNormal = quarterMs * note.multiplier;
        note.hzNormal = 1000.0f / note.msNormal;

        note.msDotted = note.msNormal * 1.5f;
        note.hzDotted = 1000.0f / note.msDotted;

        note.msTriplet = note.msNormal * (2.0f / 3.0f);
        note.hzTriplet = 1000.0f / note.msTriplet;
    }
}

void BPM2HzAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    buffer.clear();

    if (auto* playHead = getPlayHead())
    {
        if (auto position = playHead->getPosition())
        {
            if (auto bpmOpt = position->getBpm())
            {
                if (std::abs(*bpmOpt - currentBpm) > 0.001f)
                {
                    currentBpm = static_cast<float>(*bpmOpt);
                    updateTable();
                }
            }
        }
    }
}

juce::AudioProcessorEditor* BPM2HzAudioProcessor::createEditor()
{
    return new BPM2HzAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BPM2HzAudioProcessor();
}