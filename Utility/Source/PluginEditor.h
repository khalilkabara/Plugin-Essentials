/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../../Common/DisplayComponent.h"
#include "../../Common/LookAndFeel/CustomGraphics.h"


class UtilityAudioProcessorEditor : public AudioProcessorEditor,
	Slider::Listener,
	public ComboBox::Listener,
	public Button::Listener
{
public:
    UtilityAudioProcessorEditor (UtilityAudioProcessor&);
    ~UtilityAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
	
	void buttonClicked(Button* button) override
	{
		if (button == &muteToggle)
		{
			muteToggle.getToggleState() ? muteToggleSlider.setValue(1) : muteToggleSlider.setValue(0);
		}
		if (button == &loadPresetButton)
		{
			processor.loadPreset();
		}
		if (button == &savePresetButton)
		{
			processor.savePreset();
		}
		if (button == &aboutButton)
		{
			displayComponent.about(aboutText);
		}

		if (dynamic_cast<ToggleButton*>(button))
		{
			const auto val = button->getToggleState() ? "On" : "Off";
			DisplayComponent::setDisplayText(static_cast<String>(button->getName()) + ": " + val);
		}
	}

	void buttonStateChanged(Button* button) override
	{
	}

	void sliderValueChanged(Slider* slider) override
	{
		if(slider == &muteToggleSlider)
		{
			muteToggle.setToggleState(slider->getValue() == 1, dontSendNotification);
		}
		auto val = 0.0f;
		String text = "";
		if (slider == &panKnob)
		{
			text += slider->getName() + ": ";
			if (slider->getValue() == 0.0f) text += "C";
			else if(slider->getValue() > 0) text += String(slider->getValue(), 2) + "R";
			else  text += String(abs(slider->getValue()), 2) + "L";
		} else if(slider == &muteToggleSlider)
		{
			text += slider->getName() + ": ";
			text += slider->getValue() == 1 ? "On" : "Off";
		}
		else text = static_cast<String>(slider->getName()) + ": " + String(slider->getValue(), 2);

		if (slider == &gainKnob)
		{
			text += "db";
		}

		DisplayComponent::setDisplayText(text);
	}

	void comboBoxChanged(ComboBox* comboBox) override
	{
		DisplayComponent::setDisplayText(static_cast<String>(comboBox->getName()) + ": " + static_cast<String>(comboBox->getText()));
	}

private:

	Image backgroundImage;
    UtilityAudioProcessor& processor;
	CustomGraphics customGraphics;
	DisplayComponent displayComponent;

	TextButton loadPresetButton;
	TextButton savePresetButton;
	TextButton aboutButton;

	const int pluginWidth = 300;
	const int pluginHeight = 80;
	const int border = 10;
	const int margin = 5;
	const int headerHeight = 20;
	const int labelHeight = 15;
	const int aboutButtonWidth = 40;

	const String aboutText = { "Utility plugin \nBy Worq Studios \nwww.worq.com.ng" };

	int remainingHeight{ 0 };

	void defineComponents();
	void defineRects();

	//======================================

	Slider panKnob;
	Slider gainKnob;
	ToggleButton muteToggle;
	Slider muteToggleSlider;

	Rectangle<int> mainArea;
	Rectangle<int> headerRect;

	Rectangle<int> headerLeftRect;
	Rectangle<int> loadPresetButtonRect;
	Rectangle<int> savePresetButtonRect;
	Rectangle<int> headerDisplayRect;
	Rectangle<int> headerRightRect;
	Rectangle<int> muteToggleLabelRect;
	Rectangle<int> muteToggleRect;
	Rectangle<int> aboutButtonRect;

	Rectangle<int> panKnobRect;
	Rectangle<int> panKnobLabelRect;
	Rectangle<int> gainKnobRect;
	Rectangle<int> gainKnobLabelRect;

	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gainKnobAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> panKnobAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> muteToggleSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UtilityAudioProcessorEditor)
};
