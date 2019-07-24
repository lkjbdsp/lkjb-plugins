/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PitchShifter.h"

#if JUCE_WINDOWS
#pragma warning (push)
#pragma warning (disable: 4996)
#endif


class Delay
{
public:

    void setDelay(int delay)
    {
        buffer.setSize(2, delay);
        buffer.clear();
        delayIndex = 0;
    }

    void processBlock(float* chL, float* chR, int numSamples)
    {
        int index = delayIndex;
        float* dataL = buffer.getWritePointer(0);
        float* dataR = buffer.getWritePointer(1);

        for (int i=0; i<numSamples; ++i)
        {
            const float x = dataL[index];
            dataL[index++] = chL[i];

            if (index >= buffer.getNumSamples())
                index = 0;

            chL[i] = x;
        }

        if (chR != nullptr)
        {
            index = delayIndex;
            for (int i=0; i<numSamples; ++i)
            {
                const float x = dataR[index];
                dataR[index++] = chR[i];

                if (index >= buffer.getNumSamples())
                    index = 0;

                chR[i] = x;
            }
        }

        delayIndex = index;
    }

private:
    AudioSampleBuffer buffer;
    int delayIndex;
};


//==============================================================================
/**
*/
class SuperSpreadAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    SuperSpreadAudioProcessor();
    ~SuperSpreadAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;
    void processBlockBypassed (AudioBuffer<float>& buffer, MidiBuffer& midiMessages);
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    ScopedPointer<AudioProcessorValueTreeState> parameterState;

private:

    Delay mainDelay;
    AudioSampleBuffer pitchBuffer;
    OwnedArray<PitchShifter> shifter;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SuperSpreadAudioProcessor)
};

#if JUCE_WINDOWS
#pragma warning (pop)
#endif

#endif  // PLUGINPROCESSOR_H_INCLUDED
