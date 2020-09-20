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

class CompressorExpanderAudioProcessorEditor : public AudioProcessorEditor,
                                               Slider::Listener,
                                               public ComboBox::Listener,
                                               public Button::Listener
{
public:
	//==============================================================================

	CompressorExpanderAudioProcessorEditor(CompressorExpanderAudioProcessor&);
	~CompressorExpanderAudioProcessorEditor();

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
		auto text = static_cast<String>(slider->getName()) + ": " + String(slider->getValue(), 2);

		if (slider->getName() == processor.makeupGainParamName ||
			slider->getName() == processor.thresholdParamName)
		{
			text += "db";
		}
		else if (slider->getName() == processor.attackParamName ||
			slider->getName() == processor.releaseParamName)
		{
			text += "ms";
		}
		else if (slider->getName() == processor.ratioParamName)
		{
			text += ":1";
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

	CompressorExpanderAudioProcessor& processor;

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

	const int pluginWidth = 400;
	const int pluginHeight = 120;
	const int border = 10;
	const int margin = 5;
	const int headerHeight = 20;
	const int labelHeight = 15;
	const String aboutText = {"Compressor/Expander plugin \nBy Worq Studios \nwww.worq.com.ng"};

	int remainingHeight{0};

	void defineRects();

	//======================================

	Rectangle<int> mainArea;
	Rectangle<int> headerRect;

	Rectangle<int> headerLeftRect;
	Rectangle<int> loadPresetButtonRect;
	Rectangle<int> savePresetButtonRect;
	Rectangle<int> headerDisplayRect;
	Rectangle<int> headerRightRect;
	Rectangle<int> aboutButtonRect;
	Rectangle<int> bypassToggleRect;
	Rectangle<int> bypassToggleLabelRect;
	Rectangle<int> topSelectorsRect;

	Rectangle<int> modeSelectorRect;
	Rectangle<int> modeSelectorLabelRect;
	Rectangle<int> thresholdKnobRect;
	Rectangle<int> thresholdKnobLabelRect;
	Rectangle<int> ratioKnobRect;
	Rectangle<int> ratioKnobLabelRect;
	Rectangle<int> attackKnobRect;
	Rectangle<int> attackKnobLabelRect;
	Rectangle<int> releaseKnobRect;
	Rectangle<int> releaseKnobLabelRect;
	Rectangle<int> makeupGainKnobRect;
	Rectangle<int> makeupGainKnobLabelRect;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorExpanderAudioProcessorEditor)
};

//==============================================================================
