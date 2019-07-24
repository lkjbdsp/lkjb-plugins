/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <emmintrin.h>

#if JUCE_WINDOWS
#pragma warning (push)
#pragma warning (disable: 4996)
#endif

//==============================================================================
SuperSpreadAudioProcessor::SuperSpreadAudioProcessor()
{
    parameterState = new AudioProcessorValueTreeState(*this, nullptr);
    const NormalisableRange<float> gainRange(-24, 6.f);    
    std::function<String(float)> valToStr = [] (float val) { return String(val, 1); };
    std::function<float(const String&)> strToVal = [] (const String& s) { return s.getFloatValue(); };

    parameterState->createAndAddParameter("Spread", "Spread", "", NormalisableRange<float> (0.f, 100.f), 0.f, valToStr, strToVal);
    parameterState->createAndAddParameter("Mix", "Mix", "", NormalisableRange<float> (0.f, 200.f), 100.f, valToStr,  strToVal);

    for (int i=0; i<12; ++i)
        shifter.add(new PitchShifter(2048));

    parameterState->state = ValueTree("State");
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

}

void SuperSpreadAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& /*midiMessages*/)
{
    unsigned int csr = _mm_getcsr();
    _mm_setcsr(csr | 0x8040);
    AudioProcessorParameter* mixParam = parameterState->getParameter("Mix");
    const NormalisableRange<float> mixRange(parameterState->getParameterRange("Mix"));

    const float spread0 = parameterState->getParameter("Spread")->getValue();
    const float mix = mixRange.convertFrom0to1(mixParam->getValue());
    const float detuneFade = jmin(spread0/0.1f, 1.f);

    const float detunedGain = mix >= 100.f ? 1.f : mix / 100.f;
    const float dryGain = mix <= 100.f ? 1.f : detuneFade < 1.f ? jmax(0.5f * (1.f - detuneFade), (200.f - mix) / 100.f) : (200.f - mix) / 100.f;
    const float spreadGain = detunedGain * detuneFade;


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
    buffer.applyGain(dryGain);

    const float maxPitches[6] = {0.893f, 0.939f, 0.98f, 1.02f, 1.064f, 1.11f}; 

    for (int i=0; i<6; ++i)
    {
        shifter[i]->setPitch(std::pow(maxPitches[i], spread));
        shifter[i+6]->setPitch(std::pow(1.f / maxPitches[i], spread));

        float* procL = pitchBuffer.getWritePointer(i);
        float* procR = pitchBuffer.getWritePointer(i+6);

        shifter[i]->processBlock(procL, numSamples);
        buffer.addFrom(0, 0, procL, numSamples, spreadGain/* * gain*/);

        if (numChannels == 2)
        {
            shifter[i+6]->processBlock(procR, numSamples);
            buffer.addFrom(1, 0, procR, numSamples, spreadGain/* * gain*/);
        }
    }

    const float totalGain = spreadGain == 0.f ? 1.f : 1.41f / (1.f + std::sqrt(6.f) * spreadGain);

    buffer.applyGain(totalGain);

    _mm_setcsr(csr);
}

void SuperSpreadAudioProcessor::processBlockBypassed (AudioBuffer<float>& buffer, MidiBuffer& /*midiMessages*/)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    float* chL = buffer.getWritePointer(0);
    float* chR = numChannels == 2 ? buffer.getWritePointer(1) : nullptr;

    mainDelay.processBlock(chL, chR, numSamples);
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

#if JUCE_WINDOWS
#pragma warning (pop)
#endif