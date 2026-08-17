#include "PluginProcessor.h"
#include "PluginEditor.h"

BPM2HzAudioProcessor::BPM2HzAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #ifndef JucePlugin_IsMidiEffect
                      #ifndef JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
       apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

BPM2HzAudioProcessor::~BPM2HzAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout BPM2HzAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID ("sync", 1), "DAW Sync", true));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("manualBpm", 1), "Manual BPM",
        juce::NormalisableRange<float> (20.0f, 300.0f, 0.1f, 1.0f), 120.0f));

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
    juce::ignoreUnused (index);
}

const juce::String BPM2HzAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void BPM2HzAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void BPM2HzAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (sampleRate, samplesPerBlock);
    updateNoteTable (static_cast<double> (currentBpm.load()));
}

void BPM2HzAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BPM2HzAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BPM2HzAudioProcessor::updateNoteTable (double bpm)
{
    if (bpm <= 0.0)
        return;

    double quarterMs = (60.0 / bpm) * 1000.0;

    struct DivFactor { juce::String name; double factor; };
    std::vector<DivFactor> factors = {
        { "1 Bar", 4.0 },
        { "1/2",   2.0 },
        { "1/4",   1.0 },
        { "1/8",   0.5 },
        { "1/16",  0.25 },
        { "1/32",  0.125 },
        { "1/64",  0.0625 }
    };

    std::vector<NoteValueVal> tempTable;
    tempTable.reserve (factors.size());

    for (const auto& f : factors)
    {
        NoteValueVal nv;
        nv.name = f.name;

        nv.msNormal = quarterMs * f.factor;
        nv.hzNormal = 1000.0 / nv.msNormal;

        nv.msDotted = nv.msNormal * 1.5;
        nv.hzDotted = 1000.0 / nv.msDotted;

        nv.msTriplet = nv.msNormal / 1.5;
        nv.hzTriplet = 1000.0 / nv.msTriplet;

        tempTable.push_back (nv);
    }

    noteTable = tempTable;
}

void BPM2HzAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear solo dei canali di output in eccesso (se p.es. entri in Mono ed esci in Stereo)
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // NOTA: I canali audio da 0 a totalNumInputChannels - 1 NON vengono toccati.
    // L'audio passa trasparentemente dall'ingresso all'uscita (Passthrough).

    // --- Calcolo dei BPM e della tabella Hz/ms ---
    bool isSynced = apvts.getRawParameterValue ("sync")->load() > 0.5f;
    double targetBpm = 120.0;

    if (isSynced)
    {
        if (auto* playHead = getPlayHead())
        {
            if (auto positionOpt = playHead->getPosition())
            {
                if (auto bpmOpt = positionOpt->getBpm())
                {
                    targetBpm = *bpmOpt;
                }
            }
        }
    }
    else
    {
        targetBpm = static_cast<double> (apvts.getRawParameterValue ("manualBpm")->load());
    }

    currentBpm.store (static_cast<float> (targetBpm));
    updateNoteTable (targetBpm);
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