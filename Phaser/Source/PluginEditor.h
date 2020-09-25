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

class PhaserAudioProcessorEditor : public AudioProcessorEditor,
Slider::Listener,
public ComboBox::Listener,
public Button::Listener
{
public:
	//==============================================================================

	PhaserAudioProcessorEditor(PhaserAudioProcessor&);
	~PhaserAudioProcessorEditor();

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
		// const auto val = (int)(slider->getValue() * 100 + .5) / 100.0);
		// const auto val = roundf(slider->getValue() * 100) / 100;

		auto text = static_cast<String>(slider->getName()) + ": " + String(slider->getValue(), 2);

		if(slider->getName() == processor.lfoFrequencyParamName ||
			slider->getName() == processor.sweepWidthParamName || 
			slider->getName() == processor.minFrequencyParamName)
		{
			text += "Hz";
		} else if (slider->getName() == processor.gainParamName)
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
	//==============================================================================

	PhaserAudioProcessor& processor;

	enum {
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
	const int aboutButtonWidth = 40;
	const String aboutText{  "Phaser"  };

	int remainingHeight{ 0 };

	void defineRects();

	//======================================

	Rectangle<int> mainArea;
	Rectangle<int> headerRect;
	Rectangle<int> numFiltersRect;
	Rectangle<int> lfoRect;
	Rectangle<int> controlsRect;
	Rectangle<int> footerRect;

	Rectangle<int> headerLeftRect;
	Rectangle<int> loadPresetButtonRect;
	Rectangle<int> savePresetButtonRect;
	Rectangle<int> headerDisplayRect;
	Rectangle<int> headerRightRect;
	Rectangle<int> useStereoToggleLabelRect;
	Rectangle<int> useStereoToggleRect;
	Rectangle<int> aboutButtonRect;

	Rectangle<int> numFiltersLabelRect;
	Rectangle<int> numFiltersSelectorRect;

	Rectangle<int> lfoTitleRect;
	Rectangle<int> lfoWaveSelectorRect;
	Rectangle<int> lfoWaveSelectorTitleRect;
	Rectangle<int> lfoFrequencyKnobRect;
	Rectangle<int> lfoFrequencyKnobLabelRect;

	Rectangle<int> minFrequencyKnobRect;
	Rectangle<int> minFrequencyKnobLabelRect;
	Rectangle<int> sweepWidthKnobRect;
	Rectangle<int> sweepWidthKnobLabelRect;
	Rectangle<int> feedbackKnobRect;
	Rectangle<int> feedbackKnobLabelRect;
	Rectangle<int> depthKnobRect;
	Rectangle<int> depthKnobLabelRect;
	Rectangle<int> gainKnobRect;
	Rectangle<int> gainKnobLabelRect;

	Image backgroundImage;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhaserAudioProcessorEditor)
};

//==============================================================================
