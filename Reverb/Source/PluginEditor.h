/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../../Common/DisplayComponent.h"
#include "../../Common/LookAndFeel/CustomGraphics.h"


class ReverbAudioProcessorEditor : public AudioProcessorEditor,
	Slider::Listener,
	public ComboBox::Listener,
	public Button::Listener
{
public:
    ReverbAudioProcessorEditor (ReverbAudioProcessor&);
    ~ReverbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
	
	void buttonClicked(Button* button) override
	{
		if (button == &bypassToggle)
		{
			bypassToggle.getToggleState() ? bypassToggleSlider.setValue(1) : bypassToggleSlider.setValue(0);
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
		if(slider == &bypassToggleSlider)
		{
			bypassToggle.setToggleState(slider->getValue() == 1, dontSendNotification);
		}
		auto val = 0.0f;
		String text = "";
		// if (slider == &dampingKnob)
		// {
		// 	text += slider->getName() + ": ";
		// 	if (slider->getValue() == 0.0f) text += "C";
		// 	else if(slider->getValue() > 0) text += String(slider->getValue(), 2) + "R";
		// 	else  text += String(abs(slider->getValue()), 2) + "L";
		// } else 
		if(slider == &bypassToggleSlider)
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
    ReverbAudioProcessor& processor;
	CustomGraphics customGraphics;
	DisplayComponent displayComponent;

	TextButton loadPresetButton;
	TextButton savePresetButton;
	TextButton aboutButton;

	const int pluginWidth = 350;
	const int pluginHeight = 160;
	const int border = 10;
	const int margin = 5;
	const int headerHeight = 20;
	const int labelHeight = 15;
	const int aboutButtonWidth = 40;

	const String aboutText = { "Reverb plugin \nBy Worq Studios \nwww.worq.com.ng" };

	int remainingHeight{ 0 };

	void defineComponents();
	void defineRects();

	//======================================

	ToggleButton bypassToggle;
	Slider bypassToggleSlider;
	Slider dampingKnob;
	Slider roomSizeKnob;
	Slider widthKnob;
	Slider dryLevelKnob;
	Slider wetLevelKnob;
	Slider gainKnob;

	Rectangle<int> mainArea;
	Rectangle<int> headerRect;

	Rectangle<int> headerLeftRect;
	Rectangle<int> loadPresetButtonRect;
	Rectangle<int> savePresetButtonRect;
	Rectangle<int> headerDisplayRect;
	Rectangle<int> headerRightRect;
	Rectangle<int> bypassToggleLabelRect;
	Rectangle<int> bypassToggleRect;
	Rectangle<int> aboutButtonRect;

	Rectangle<int> dampingKnobRect;
	Rectangle<int> dampingKnobLabelRect;
	Rectangle<int> roomSizeKnobRect;
	Rectangle<int> roomSizeKnobLabelRect;
	Rectangle<int> widthKnobRect;
	Rectangle<int> widthKnobLabelRect;
	Rectangle<int> dryLevelKnobRect;
	Rectangle<int> dryLevelKnobLabelRect;
	Rectangle<int> wetLevelKnobRect;
	Rectangle<int> wetLevelKnobLabelRect;
	Rectangle<int> gainKnobRect;
	Rectangle<int> gainKnobLabelRect;

	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> bypassToggleSliderAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dampingKnobAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> roomSizeKnobAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> widthKnobAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dryLevelKnobAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> wetLevelKnobAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gainKnobAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbAudioProcessorEditor)
};
