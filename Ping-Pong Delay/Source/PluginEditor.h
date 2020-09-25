/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/


#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "../../Common/DisplayComponent.h"
#include "../../Common/LookAndFeel/CustomGraphics.h"

//==============================================================================

class PingPongDelayAudioProcessorEditor : public AudioProcessorEditor,
                                          Slider::Listener,
                                          public ComboBox::Listener,
                                          public Button::Listener
{
public:
	//==============================================================================

	PingPongDelayAudioProcessorEditor(PingPongDelayAudioProcessor&);
	~PingPongDelayAudioProcessorEditor();

	//==============================================================================

	void paint(Graphics&) override;
	void resized() override;

	void buttonClicked(Button* button) override
	{
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
		auto val = 0.0f;
		String text = "";
		if (slider->getName() == processor.balanceParamName)
		{
			text += slider->getName() + ": ";
			if (slider->getValue() == 0.5f) text += "C";
			else if(slider->getValue() > 0.5f)
			{
				val = jmap<float>(slider->getValue(), 0.5, 1, 0, 1);
				text += String(val, 2) + "R";
			} else
			{
				val = jmap<float>(slider->getValue(), 0.0, 0.5, -1, 0);
				text += String(abs(val), 2) + "L";
			}
		}
		else text = static_cast<String>(slider->getName()) + ": " + String(slider->getValue(), 2);

		if (slider->getName() == processor.gainParamName)
		{
			text += "db";
		}
		else if (slider->getName() == processor.delayTimeParamName)
		{
			text += "s";
		}

		DisplayComponent::setDisplayText(text);
	}

	void comboBoxChanged(ComboBox* comboBox) override
	{
		DisplayComponent::setDisplayText(
			static_cast<String>(comboBox->getName()) + ": " + static_cast<String>(comboBox->getText()));
	}

private:
	//==============================================================================

	PingPongDelayAudioProcessor& processor;

	enum
	{
		editorWidth = 500,
		editorMargin = 10,
		editorPadding = 10,

		sliderTextEntryBoxWidth = 100,
		sliderTextEntryBoxHeight = 25,
		sliderHeight = 25,
		buttonHeight = 25,
		comboBoxHeight = 25,
		labelWidth = 100,
	};

	//======================================

	OwnedArray<Slider> sliders;
	OwnedArray<ToggleButton> toggles;
	OwnedArray<ComboBox> comboBoxes;

	OwnedArray<Label> labels;
	Array<Component*> components;

	typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
	typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
	typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

	OwnedArray<SliderAttachment> sliderAttachments;
	OwnedArray<ButtonAttachment> buttonAttachments;
	OwnedArray<ComboBoxAttachment> comboBoxAttachments;

	//******************************************************************************************

	Image backgroundImage;
	CustomGraphics customGraphics;
	DisplayComponent displayComponent;

	TextButton loadPresetButton;
	TextButton savePresetButton;
	TextButton aboutButton;

	const int numKnobs{5};

	const int pluginWidth = 350;
	const int pluginHeight = 100;
	const int border = 10;
	const int margin = 5;
	const int headerHeight = 20;
	const int labelHeight = 15;
	const String aboutText { "Ping-Pong Delay" };

	int remainingHeight{0};

	void defineRects();

	//======================================

	Rectangle<int> mainArea;
	Rectangle<int> headerRect;

	Rectangle<int> headerLeftRect;
	Rectangle<int> loadPresetButtonRect;
	Rectangle<int> savePresetButtonRect;
	Rectangle<int> aboutButtonRect;
	Rectangle<int> headerDisplayRect;
	Rectangle<int> headerRightRect;

	Rectangle<int> gainKnobRect;
	Rectangle<int> gainKnobLabelRect;
	Rectangle<int> delayTimeKnobRect;
	Rectangle<int> delayTimeKnobLabelRect;
	Rectangle<int> feedbackKnobRect;
	Rectangle<int> feedbackKnobLabelRect;
	Rectangle<int> mixKnobRect;
	Rectangle<int> mixKnobLabelRect;
	Rectangle<int> balanceKnobRect;
	Rectangle<int> balanceKnobLabelRect;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PingPongDelayAudioProcessorEditor)
};

//==============================================================================
