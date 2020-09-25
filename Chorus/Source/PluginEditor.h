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

class ChorusAudioProcessorEditor : public AudioProcessorEditor,
	Slider::Listener,
	public ComboBox::Listener,
	public Button::Listener
{
public:
	//==============================================================================

	ChorusAudioProcessorEditor(ChorusAudioProcessor&);
	~ChorusAudioProcessorEditor();

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

		if (slider->getName() == processor.lfoFrequencyParamName)
		{
			text += "Hz";
		}
		else if (slider->getName() == processor.gainParamName)
		{
			text += "db";
		}
		else if (slider->getName() == processor.delayParamName ||
			slider->getName() == processor.widthParamName)
		{
			text += "ms";
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

	ChorusAudioProcessor& processor;

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

	CustomGraphics customGraphics;
	DisplayComponent displayComponent;

	TextButton loadPresetButton;
	TextButton savePresetButton;
	TextButton aboutButton;

	const int pluginWidth = 400;
	const int pluginHeight = 200;
	const int border = 10;
	const int margin = 5;
	const int headerHeight = 20;
	const int labelHeight = 15;
	const String aboutText{ "Chorus" };

	int remainingHeight{ 0 };

	void defineRects();

	Image backgroundImage;

	//======================================

	Rectangle<int> mainArea;
	Rectangle<int> headerRect;
	Rectangle<int> topSelectorsRect;
	Rectangle<int> lfoRect;
	Rectangle<int> controlsRect;
	Rectangle<int> footerRect;

	Rectangle<int> headerLeftRect;
	Rectangle<int> loadPresetButtonRect;
	Rectangle<int> savePresetButtonRect;
	Rectangle<int> aboutButtonRect;
	Rectangle<int> headerDisplayRect;
	Rectangle<int> headerRightRect;
	Rectangle<int> useStereoToggleLabelRect;
	Rectangle<int> useStereoToggleRect;
	Rectangle<int> numVoicesLabelRect;
	Rectangle<int> numVoicesSelectorRect;

	Rectangle<int> interpolationLabelRect;
	Rectangle<int> interpolationSelectorRect;

	Rectangle<int> lfoTitleRect;
	Rectangle<int> lfoWaveSelectorRect;
	Rectangle<int> lfoWaveSelectorTitleRect;
	Rectangle<int> lfoFrequencyKnobRect;
	Rectangle<int> lfoFrequencyKnobLabelRect;

	Rectangle<int> delayKnobRect;
	Rectangle<int> delayKnobLabelRect;
	Rectangle<int> widthKnobRect;
	Rectangle<int> widthKnobLabelRect;
	Rectangle<int> depthKnobRect;
	Rectangle<int> depthKnobLabelRect;
	Rectangle<int> gainKnobRect;
	Rectangle<int> gainKnobLabelRect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChorusAudioProcessorEditor)
};

//==============================================================================
