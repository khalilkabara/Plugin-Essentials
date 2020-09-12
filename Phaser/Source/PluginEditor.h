/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "../../Common/LookAndFeel/CustomGraphics.h"

//==============================================================================

class PhaserAudioProcessorEditor : public AudioProcessorEditor,
                                   public Slider::Listener,
                                   public ToggleButton::Listener,
                                   public ComboBox::Listener
{
public:
	//==============================================================================

	PhaserAudioProcessorEditor(PhaserAudioProcessor&);
	~PhaserAudioProcessorEditor();

	//==============================================================================

	void paint(Graphics&) override;
	void resized() override;

	void buttonClicked(Button*) override
	{
	}

	void buttonStateChanged(Button* button) override
	{
		if (button == &useStereoToggle)
		{
			useStereoToggle.getToggleState() ? useStereoToggleSlider.setValue(1) : useStereoToggleSlider.setValue(0);
		}
	}

	void sliderValueChanged(Slider* slider) override
	{
	}

	void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override
	{
		
	}
	
private:
	//==============================================================================

	CustomGraphics customGraphics;
	PhaserAudioProcessor& processor;

	const int pluginWidth = 400;
	const int pluginHeight = 200;
	const int border = 10;
	const int margin = 5;
	const int headerHeight = 20;
	const int labelHeight = 15;

	int remainingHeight{0};

	void defineRects();
	void defineComponents();
	void addComponents();

	//======================================

	Rectangle<int> mainArea;
	Rectangle<int> headerRect;
	Rectangle<int> numFiltersRect;
	Rectangle<int> lfoRect;
	Rectangle<int> controlsRect;
	Rectangle<int> footerRect;
	
	Rectangle<int> headerLeftRect;
	Rectangle<int> headerDisplayRect;
	Rectangle<int> headerRightRect;
	Rectangle<int> useStereoToggleLabelRect;
	Rectangle<int> useStereoToggleRect;

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
	Rectangle<int> dryWetKnobRect;
	Rectangle<int> dryWetKnobLabelRect;

	ToggleButton useStereoToggle;
	Slider useStereoToggleSlider;
	ComboBox numFiltersSelector;
	ComboBox lfoWaveSelector;
	Slider depthKnob;
	Slider feedbackKnob;
	Slider minFrequencyKnob;
	Slider sweepWidthKnob;
	Slider lfoFrequencyKnob;
	Slider dryWetKnob;

	//==============================================================================

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhaserAudioProcessorEditor)
};

//==============================================================================
