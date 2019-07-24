#ifndef __PITCHSHIFTER__
#define __PITCHSHIFTER__


#include "../../../Dirac_LE/src/Dirac.h"
#include "JuceHeader.h"


class PitchShifter
{
public:
	PitchShifter(float sampleRate = 44100.)
		: _shifter(0),
			_pitch(1.f),
			latency(0)
	{
		_shifter = DiracFxCreate(kDiracQualityBest, sampleRate, 1);
		jassert(_shifter != 0);
		latency = DiracFxLatencyFrames(sampleRate);
	}

	~PitchShifter()
	{
		if (_shifter != 0)
			DiracFxDestroy(_shifter);
	}


	int getLatency()
	{
		return latency;
	}

	void setPitch(double newPitch)
	{
		// max 3 octave up or down.
		_pitch = jlimit(0.125, 8., newPitch);
	}

	void setPitchSemitones(double semitones)
	{
		_pitch = jlimit(0.125, 8., pow(2., semitones / 12.));
	}


	void processBlock(float* data, int numSamples)
	{
		if (_shifter != 0)
		{
			for (int i=0; i<numSamples; ++i)
				data[i] *= 0.125f;

			DiracFxProcessFloat(1., _pitch, &data, &data, numSamples, _shifter);

			for (int i=0; i<numSamples; ++i)
				data[i] *= 8.f;

		}
	}


private:

	void* _shifter;
	double _pitch;

	int latency;

};



#endif