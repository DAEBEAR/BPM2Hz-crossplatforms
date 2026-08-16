#include "PluginProcessor.h"
#include "PluginEditor.h"

BPM2HzAudioProcessor::BPM2HzAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BugsChannelSet (juce::AudioChannelSet::disabled(), juce::AudioChannelSet::disabled())),
#endif
       apvts (*this, nullptr, "Parameters", createParameterLayout())
{
    updateBpmTable (120.0f);
}

BPM2HzAudioProcessor::~BPM2HzAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout BPM2HzAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID ("sync", 1),
        "DAW Sync",
        true));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID ("manualBpm", 1),
        "Manual BPM",
        juce::NormalisableRange<float> (20.0f, 300.0f, 0.1f, 1.0f),
        120.0f));

    return { params.begin(), params.end() };
}

const juce::String BPM2HzAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BPM2HzAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BPM2HzAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BPM2HzAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BPM2HzAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BPM2HzAudioProcessor::getNumPrograms()
{
    return 1;
}

int BPM2HzAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BPM2HzAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BPM2HzAudioProcessor::getProgramName (int index)
{
    return {};
}

void BPM2HzAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void BPM2HzAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void BPM2HzAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BPM2HzAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return true;
}
#endif

void BPM2HzAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    bool isSynced = apvts.getRawParameterValue ("sync")->load() > 0.5f;
    float bpmToUse = apvts.getRawParameterValue ("manualBpm")->load();

    if (isSynced)
    {
        if (auto* playHead = getPlayHead())
        {
            if (auto position = playHead->getPosition())
            {
                if (auto bpmOpt = position->getBpm())
                {
                    bpmToUse = static_cast<float> (*bpmOpt);
                }
            }
        }
    }

    currentBpm = bpmToUse;
    updateBpmTable (currentBpm);
}

void BPM2HzAudioProcessor::updateBpmTable (float bpm)
{
    if (bpm <= 0.0f)
        return;

    // Durata di un quarto di nota (Quarter Note / 1/4) in millisecondi
    double quarterNoteMs = (60.0 / static_cast<double>(bpm)) * 1000.0;

    struct DivisionInfo {
        juce::String name;
        double factor; // Fattore moltiplicativo rispetto alla semiminima (1/4)
    };

    std::vector<DivisionInfo> divisions = {
        { "1 Bar", 4.0 },
        { "1/2",   2.0 },
        { "1/4",   1.0 },
        { "1/8",   0.5 },
        { "1/16",  0.25 },
        { "1/32",  0.125 },
        { "1/64",  0.0625 }
    };

    noteTable.clear();

    for (const auto& div : divisions)
    {
        NoteValueVal note;
        note.name = div.name;

        // Valore normale
        note.msNormal = quarterNoteMs * div.factor;
        note.hzNormal = 1000.0 / note.msNormal;

        // Valore col punto (Dotted = x 1.5)
        note.msDotted = note.msNormal * 1.5;
        note.hzDotted = 1000.0 / note.msDotted;

        // Valore terzinato (Triplet = x 2/3)
        note.msTriplet = note.msNormal * (2.0 / 3.0);
        note.hzTriplet = 1000.0 / note.msTriplet;

        noteTable.push_back (note);
    }
}

bool BPM2HzAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* BPM2HzAudioProcessor::createEditor()
{
    return new BPM2HzAudioProcessorEditor (*this);
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

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BPM2HzAudioProcessor();
}