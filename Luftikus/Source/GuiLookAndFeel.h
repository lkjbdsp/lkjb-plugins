#ifndef __GUILOOKANDFEEL_H_A272DB78__
#define __GUILOOKANDFEEL_H_A272DB78__

#include "JuceHeader.h"

class GuiLookAndFeel : public LookAndFeel_V2
{
public:
	GuiLookAndFeel();
	void drawTickBox (Graphics& g,
                              Component& component,
                              float x, float y, float w, float h,
                              bool ticked,
                              bool isEnabled,
                              bool /*isMouseOverButton*/,
                              bool /*isButtonDown*/);


	void drawRotarySlider (Graphics& g,
                           int x, int y,
                           int width, int height,
                           float sliderPos,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           Slider& slider);

	void drawToggleButton (Graphics& g,
	                       ToggleButton& button,
	                       bool isMouseOverButton,
	                       bool isButtonDown);
private:
	Image chickenKnob;
	int numChickenImages;

	Image masterKnob;
	int numTrimImages;

	Image toggleOff;
	Image toggleOn;
	Image switchOff;
	Image switchOn;

};


class GuiSlider : public Slider,
                  public Label::Listener,
                  public Slider::Listener
{
public:
	explicit GuiSlider (const String& componentName = "");

	GuiSlider (SliderStyle style, TextEntryBoxPosition textBoxPosition);

	void resized();

	void mouseDown(const MouseEvent& e);

	void labelTextChanged(Label* labelThatHasChanged);

	void sliderValueChanged(Slider* slider);

	void setLabelText();

private:

	double resetValue;
	Label label;
};


#endif  // __GUILOOKANDFEEL_H_A272DB78__
