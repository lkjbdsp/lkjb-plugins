/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

class SuperSpreadLookAndFeel;

//==============================================================================
/**
*/
class SuperSpreadAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    SuperSpreadAudioProcessorEditor (SuperSpreadAudioProcessor&);
    ~SuperSpreadAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

	void mouseDown(const MouseEvent& e) override;

private:
    
    SuperSpreadAudioProcessor& processor;

    Slider spread;
	Slider mix;
    SharedResourcePointer<SuperSpreadLookAndFeel> lookAndFeel;
    OwnedArray<AudioProcessorValueTreeState::SliderAttachment> sliderAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SuperSpreadAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
