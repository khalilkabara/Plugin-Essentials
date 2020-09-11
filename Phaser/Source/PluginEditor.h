/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "LookAndFeel/CustomGraphics.h"

//==============================================================================

class PhaserAudioProcessorEditor : public AudioProcessorEditor
{
public:
    //==============================================================================

    PhaserAudioProcessorEditor (PhaserAudioProcessor&);
    ~PhaserAudioProcessorEditor();

    //==============================================================================

    void paint (Graphics&) override;
    void resized() override;

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

    //======================================

	// float paramDepth;
	// float paramFeedback;
	// float paramNumFilters;
	// float paramMinFrequency;
	// float paramSweepWidth;
	// float paramLFOfrequency;
	// float paramLFOwaveform;
	// float paramStereo;

	Rectangle<int> mainArea;
	Rectangle<int> headerRect;
	Rectangle<int> numFiltersRect;
	Rectangle<int> lfoRect;
	Rectangle<int> controlsRect;
	Rectangle<int> footerRect;

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
	

    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaserAudioProcessorEditor)
};

//==============================================================================
