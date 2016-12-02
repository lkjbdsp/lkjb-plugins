/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <emmintrin.h>

//==============================================================================
SuperSpreadAudioProcessor::SuperSpreadAudioProcessor()
//#ifndef JucePlugin_PreferredChannelConfigurations
//     : AudioProcessor (BusesProperties()
//                     #if ! JucePlugin_IsMidiEffect
//                      #if ! JucePlugin_IsSynth
//                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
//                      #endif
//                       .withOutput ("Output", AudioChannelSet::stereo(), true)
//                     #endif
//                       )
//#endif
{
    parameterState = new AudioProcessorValueTreeState(*this, nullptr);
    parameterState->createAndAddParameter("Spread", "Spread", "", NormalisableRange<float> (0.f, 100.f), 0.f, nullptr, nullptr);

    for (int i=0; i<12; ++i)
        shifter.add(new PitchShifter(2048));
}

SuperSpreadAudioProcessor::~SuperSpreadAudioProcessor()
{
}

//==============================================================================
const String SuperSpreadAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SuperSpreadAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SuperSpreadAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double SuperSpreadAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SuperSpreadAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SuperSpreadAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SuperSpreadAudioProcessor::setCurrentProgram (int /*index*/)
{
}

const String SuperSpreadAudioProcessor::getProgramName (int /*index*/)
{
    return String();
}

void SuperSpreadAudioProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void SuperSpreadAudioProcessor::prepareToPlay (double /*sampleRate*/, int samplesPerBlock)
{
    pitchBuffer.setSize(12, samplesPerBlock);
    setLatencySamples(shifter[0]->getLatencySamples());
    mainDelay.setDelay(getLatencySamples());   
}

void SuperSpreadAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

//#ifndef JucePlugin_PreferredChannelConfigurations
//bool SuperSpreadAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
//{
//  #if JucePlugin_IsMidiEffect
//    ignoreUnused (layouts);
//    return true;
//  #else
//    // This is the place where you check if the layout is supported.
//    // In this template code we only support mono or stereo.
//    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
//     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
//        return false;
//
//    // This checks if the input layout matches the output layout
//   #if ! JucePlugin_IsSynth
//    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
//        return false;
//   #endif
//
//    return true;
//  #endif
//}
//#endif

void SuperSpreadAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& /*midiMessages*/)
{
    unsigned int csr = _mm_getcsr();
    _mm_setcsr(csr | 0x8040);

    const float spread0 = parameterState->getParameter("Spread")->getValue();
    //const float spreadGain = std::pow(spread0, 0.1f);
    const float spreadGain = jmin(spread0/0.1f, 1.f);

    const float spread = 0.5f * spread0*spread0;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    float* chL = buffer.getWritePointer(0);
    float* chR = numChannels == 2 ? buffer.getWritePointer(1) : nullptr;

    for (int i=0; i<12 / 2; ++i)
    {
        pitchBuffer.copyFrom(i, 0, chL, numSamples);

        if (chR != nullptr)
            pitchBuffer.copyFrom(6 + i, 0, chR, numSamples);
    }

    mainDelay.processBlock(chL, chR, numSamples);

    const float maxPitches[6] = {0.893f, 0.939f, 0.98f, 1.02f, 1.064f, 1.11f}; 

    for (int i=0; i<6; ++i)
    {
        shifter[i]->setPitch(std::pow(maxPitches[i], spread));
        shifter[i+6]->setPitch(std::pow(1.f / maxPitches[i], spread));

        float* procL = pitchBuffer.getWritePointer(i);
        float* procR = pitchBuffer.getWritePointer(i+6);

        shifter[i]->processBlock(procL, numSamples);
        buffer.addFrom(0, 0, procL, numSamples, spreadGain);

        if (numChannels == 2)
        {
            shifter[i+6]->processBlock(procR, numSamples);
            buffer.addFrom(1, 0, procR, numSamples, spreadGain);
        }
    }

    const float totalGain = spreadGain == 0.f ? 1.f : 1.41f / (1.f + std::sqrt(6.f) * spreadGain);

    buffer.applyGain(totalGain);

    _mm_setcsr(csr);
}

//==============================================================================
bool SuperSpreadAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SuperSpreadAudioProcessor::createEditor()
{
    return new SuperSpreadAudioProcessorEditor (*this);
}

//==============================================================================
void SuperSpreadAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    XmlElement xml("STATE");

    for (int i=0; i<getNumParameters(); ++i)
        xml.setAttribute(getParameterName(i), getParameter(i));

    copyXmlToBinary(xml, destData);
}

void SuperSpreadAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ScopedPointer<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml != nullptr)
    {
        for (int i=0; i<getNumParameters(); ++i)
            setParameterNotifyingHost(i, static_cast<float> (xml->getDoubleAttribute(getParameterName(i), getParameter(i))));
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SuperSpreadAudioProcessor();
}
