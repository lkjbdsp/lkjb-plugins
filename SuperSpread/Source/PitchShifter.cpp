#include "PitchShifter.h"

PitchShifter::PitchShifter(int bufmax)
    : bufferSize(bufmax),
      buf(bufferSize+1),
      win(bufferSize +1)

{
    for(int i=0;i<bufferSize;i++) 
        win[i] = 0.5f - 0.5f * std::cos(2.f * i * float_Pi / bufferSize); 

    clear();
}

void PitchShifter::clear()
{
    posInput = 0; 
    posOutput = Random::getSystemRandom().nextDouble() * (bufferSize - 1);

    for(int i=0;i<bufferSize;i++) 
        buf[i] = 0;
}

void PitchShifter::setPitch(float newPitch)
{
    outDelta = newPitch;
}

void PitchShifter::processBlock(float* data, int numSamples)
{
    double p1f;
    double p2 = posOutput;
    double delta2 = outDelta;
    int  p0 = posInput;
    int p1i;
    int p2i;
    int  l = bufferSize-1;
    int lh = bufferSize>>1;
    float lf = static_cast<float> (bufferSize);

    for (int i=0; i<numSamples; ++i)
    {
        --p0 &= l;
        buf[p0] = data[i];

        p2 -= delta2;
        if(p2<0.0f) p2 += lf;

        jassert(p2 >= 0 && p2 < bufferSize);

        p1i = static_cast<int> (p2);
        p1f = p2 - static_cast<float> (p1i);
        const float x3a = buf[p1i];
        ++p1i &= l;
        const float x3b = buf[p1i];
        const float x3 = static_cast<float> (x3a + p1f * (x3b - x3a));

        p2i = (p1i + lh) & l;
        const float x4a = buf[p2i];
        ++p2i &= l;
        const float x4b = buf[p2i];
        const float x4 = static_cast<float> (x4a + p1f * (x4b - x4a));

        p2i = (p1i - p0) & l;
        const float fade34 = win[p2i];
        const float output = x4 + fade34 * (x3 - x4);

        data[i] = output;
    }

    posInput = p0; 
    posOutput = p2;
}

int PitchShifter::getLatencySamples()
{
    return bufferSize / 2;
}	