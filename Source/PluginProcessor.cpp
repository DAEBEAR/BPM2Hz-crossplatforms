#include "PluginProcessor.h"
#include "PluginEditor.h"

BPM2HzAudioProcessor::BPM2HzAudioProcessor()
    : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
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

juce::AudioProcessorValueTreeState::ParameterLayout BPM2HzAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "sync", 1 }, "Sync to DAW", true));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "manualBpm", 1 }, "Manual BPM",
        juce::NormalisableRange<float> (20.0f, 300.0f, 0.1f), 120.0f));

    return { params.begin(), params.end() };
}

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

    bool syncEnabled = apvts.getRawParameterValue ("sync")->load() > 0.5f;

    if (syncEnabled)
    {
        if (auto* ph = getPlayHead())
        {
            if (auto position = ph->getPosition())
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
    else
    {
        float manualBpm = apvts.getRawParameterValue ("manualBpm")->load();
        if (std::abs(manualBpm - currentBpm) > 0.001f)
        {
            currentBpm = manualBpm;
            updateTable();
        }
    }
}

void BPM2HzAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void BPM2HzAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessorEditor* BPM2HzAudioProcessor::createEditor()
{
    return new BPM2HzAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BPM2HzAudioProcessor();
}