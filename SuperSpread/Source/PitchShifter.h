#ifndef PITCHSHIFTER_H_INCLUDED
#define PITCHSHIFTER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

// Simple pitch shifter class using moving delay taps and a hann window. This is based on the algorithm used in mda Detune (http://mda.smartelectronix.com/)
class PitchShifter
{
public:
    PitchShifter(int bufmax = 4096);

    void clear();
    void setPitch(float newPitch);
    void processBlock(float* data, int numSamples);
    int getLatencySamples();

private:

    int bufferSize;
    float sampleRate;
    HeapBlock<float> buf;
    HeapBlock<float> win;

    int  posInput;
    double posOutput;
    double outDelta;
};



#endif  // PITCHSHIFTER_H_INCLUDED
