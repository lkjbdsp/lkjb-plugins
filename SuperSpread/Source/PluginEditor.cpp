/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

class SuperSpreadLookAndFeel : public LookAndFeel_V3
{
public:
    SuperSpreadLookAndFeel()
        : knob(ImageCache::getFromMemory(BinaryData::knob_png, BinaryData::knob_pngSize))
    {
    }

    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos, float /*rotaryStartAngle*/, float /*rotaryEndAngle*/, Slider& /*slider*/) override
    {
        const int yOffset = knob.getWidth() * static_cast<int> (sliderPos * (knob.getHeight() - knob.getWidth()) / knob.getWidth());
        Image img(knob.getClippedImage(juce::Rectangle<int> (0, yOffset, knob.getWidth(), knob.getWidth())));

        g.setColour(Colours::black);
        g.drawImage(img, juce::Rectangle<int> (x, y, width, height).toFloat(), RectanglePlacement::centred);
    }

private:
    Image knob;
};


//==============================================================================
SuperSpreadAudioProcessorEditor::SuperSpreadAudioProcessorEditor (SuperSpreadAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setLookAndFeel(lookAndFeel);
    addAndMakeVisible(spread);
    spread.setSliderStyle(Slider::RotaryVerticalDrag);
    spread.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    spread.setColour(Label::outlineColourId, Colours::transparentBlack);

    addChildComponent(mix);
    mix.setSliderStyle(Slider::RotaryVerticalDrag);
	mix.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    mix.setColour(Label::outlineColourId, Colours::transparentBlack);

    sliderAttachments.add(new AudioProcessorValueTreeState::SliderAttachment(*processor.parameterState, "Spread", spread));
    sliderAttachments.add(new AudioProcessorValueTreeState::SliderAttachment(*processor.parameterState, "Mix", mix));
    spread.setRange(spread.getMinimum(), spread.getMaximum(), 0.1);    
    mix.setRange(mix.getMinimum(), mix.getMaximum(), 0.1);    

    setSize (320, 120);
}

SuperSpreadAudioProcessorEditor::~SuperSpreadAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void SuperSpreadAudioProcessorEditor::paint (Graphics& g)
{
    const int w = getWidth();
    const int h = getHeight();
    const Colour col1(0xFF808080);
    const Colour col2(0xFFF0F0F0);
    ColourGradient grad(col1, 0.f, 0.f, col1, 0.f, static_cast<float> (h), false);
    grad.addColour(0.1, col2);
    grad.addColour(0.9, col2);
    g.setGradientFill(grad);
    g.fillAll();


    const int x0 = spread.getRight() + 10;
    const int w0 = w - x0 - 10;

    const Font bold(Font(0.4f*h).boldened());
    const Font plain(Font(0.2f*h));
    g.setFont(bold);
    g.setColour(Colours::black);
    g.drawText("Super", x0, 0, w0, h/2, Justification::bottomRight);
    g.drawText(mix.isVisible() ? "Mix" : "Spread", x0, h/2, w0, h/2, Justification::topRight);

    if (mix.isVisible())
    {
        g.setFont(10.f);
        g.drawText("Click to switch back to Spread control", 0, h-15, w-10, 15, Justification::centredRight);
    }

    const int w1 = plain.getStringWidth("lkjb");
    const int x1 = x0 + w0 - bold.getStringWidth("Super") - w1 - 10;
    g.setFont(plain);
    g.setColour(Colours::black.withAlpha(0.8f));
    g.drawText("lkjb", x1, 0, w1, h*6/13, Justification::bottomRight);
}

void SuperSpreadAudioProcessorEditor::resized()
{
	spread.setBounds(10, 10, 80, 100);
	mix.setBounds(10, 10, 80, 100);
}

void SuperSpreadAudioProcessorEditor::mouseDown (const MouseEvent& /*e*/)
{
	if (mix.isVisible())
	{
		mix.setVisible(false);
		spread.setVisible(true);
	}
	else
	{
		mix.setVisible(true);
		spread.setVisible(false);
	}
	repaint();
}
