/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/


#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

PhaserAudioProcessorEditor::PhaserAudioProcessorEditor(PhaserAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p)
{
	setSize(pluginWidth + border * 2, pluginHeight + border);
	setLookAndFeel(&customGraphics);

	defineRects();
	defineComponents();
}

PhaserAudioProcessorEditor::~PhaserAudioProcessorEditor()
{
}

//==============================================================================

void PhaserAudioProcessorEditor::paint(Graphics& g)
{
	g.setColour(Colours::whitesmoke);
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

	g.drawRect(mainArea);
	g.drawRect(headerRect);
	g.drawRect(numFiltersRect);
	g.drawRect(lfoRect);
	g.drawRect(controlsRect);
	g.drawRect(footerRect);
	//--------------------><
	g.drawRect(numFiltersSelectorRect);

	g.drawRect(lfoTitleRect);
	g.drawRect(lfoWaveSelectorTitleRect);
	g.drawRect(lfoWaveSelectorRect);
	g.drawRect(lfoFrequencyKnobRect);
	g.drawRect(lfoFrequencyKnobLabelRect);

	g.drawRect(minFrequencyKnobRect);
	g.drawRect(minFrequencyKnobLabelRect);
	g.drawRect(sweepWidthKnobRect);
	g.drawRect(sweepWidthKnobLabelRect);
	g.drawRect(feedbackKnobRect);
	g.drawRect(feedbackKnobLabelRect);
	g.drawRect(depthKnobRect);
	g.drawRect(depthKnobLabelRect);
	g.drawRect(dryWetKnobRect);
	g.drawRect(dryWetKnobLabelRect);

	//--------------------><
	g.drawFittedText("Num Filters", numFiltersLabelRect, Justification::centred, 1);

	g.drawFittedText("LFO", lfoTitleRect, Justification::centred, 1);
	g.drawFittedText("Wave", lfoWaveSelectorTitleRect, Justification::centred, 1);
	g.drawFittedText("LFO frequency", lfoFrequencyKnobLabelRect, Justification::centred, 1);

	g.drawFittedText("Frequency", minFrequencyKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Width", sweepWidthKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Feedback", feedbackKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Depth", depthKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Dry/Wet", dryWetKnobLabelRect, Justification::centred, 1);
}

void PhaserAudioProcessorEditor::resized()
{
}

void PhaserAudioProcessorEditor::defineRects()
{
	// Main rects

	mainArea = Rectangle<int>(
		getLocalBounds().getX() + border,
		getLocalBounds().getY(),
		getLocalBounds().getWidth() - border * 2,
		getLocalBounds().getHeight() - border);

	remainingHeight = mainArea.getHeight();

	headerRect = Rectangle<int>(
		mainArea.getX(),
		mainArea.getY(),
		mainArea.getWidth(),
		headerHeight);

	remainingHeight -= headerRect.getHeight() + margin;

	numFiltersRect = Rectangle<int>(
		headerRect.getX(),
		headerRect.getY() + headerRect.getHeight() + margin,
		headerRect.getWidth(),
		remainingHeight / 8);

	remainingHeight -= numFiltersRect.getHeight() + margin;

	lfoRect = Rectangle<int>(
		mainArea.getX(),
		numFiltersRect.getY() + numFiltersRect.getHeight() + margin,
		mainArea.getWidth() / 3,
		// remainingHeight * 4 / 5);
		remainingHeight);

	remainingHeight -= lfoRect.getHeight() + margin;

	controlsRect = Rectangle<int>(
		lfoRect.getX() + lfoRect.getWidth() + margin,
		lfoRect.getY(),
		mainArea.getWidth() * 2 / 3 - margin,
		lfoRect.getHeight());

	// footerRect = Rectangle<int>(
	// 	lfoRect.getX(),
	// 	lfoRect.getY() + lfoRect.getHeight() + margin,
	// 	mainArea.getWidth(),
	// 	remainingHeight);

	// ***************************************************************************************

	numFiltersLabelRect = Rectangle<int>(
		numFiltersRect.getX(),
		numFiltersRect.getY(),
		numFiltersRect.getWidth() / 3,
		numFiltersRect.getHeight());

	numFiltersSelectorRect = Rectangle<int>(
		numFiltersLabelRect.getX() + numFiltersLabelRect.getWidth(),
		numFiltersLabelRect.getY(),
		numFiltersRect.getWidth() * 2 / 3,
		numFiltersLabelRect.getHeight());

	//----------------------------------

	remainingHeight = lfoRect.getHeight();

	lfoTitleRect = Rectangle<int>(
		lfoRect.getX(),
		lfoRect.getY(),
		lfoRect.getWidth(),
		lfoRect.getHeight() / 5);

	remainingHeight -= lfoTitleRect.getHeight() + margin;

	lfoWaveSelectorTitleRect = Rectangle<int>(
		lfoTitleRect.getX() + margin,
		lfoTitleRect.getY() + lfoTitleRect.getHeight() + margin,
		lfoRect.getWidth() / 3 - margin,
		remainingHeight / 4);

	remainingHeight -= lfoWaveSelectorTitleRect.getHeight() + margin;

	lfoWaveSelectorRect = Rectangle<int>(
		lfoWaveSelectorTitleRect.getX() + lfoWaveSelectorTitleRect.getWidth() + margin,
		lfoWaveSelectorTitleRect.getY(),
		lfoRect.getWidth() * 2 / 3 - margin * 2,
		lfoWaveSelectorTitleRect.getHeight());

	lfoFrequencyKnobRect = Rectangle<int>(
		lfoWaveSelectorTitleRect.getX(),
		lfoWaveSelectorTitleRect.getY() + lfoWaveSelectorTitleRect.getHeight() + margin,
		lfoRect.getWidth() - margin * 2,
		remainingHeight - (labelHeight + margin));

	remainingHeight -= lfoFrequencyKnobRect.getHeight() + margin;

	lfoFrequencyKnobLabelRect = Rectangle<int>(
		lfoFrequencyKnobRect.getX(),
		lfoFrequencyKnobRect.getY() + lfoFrequencyKnobRect.getHeight(),
		lfoFrequencyKnobRect.getWidth(),
		labelHeight);

	//-----------------------------------

	remainingHeight = controlsRect.getHeight();

	minFrequencyKnobRect = Rectangle<int>(
		controlsRect.getX() + margin,
		controlsRect.getY() + margin,
		controlsRect.getWidth() / 3 - margin * 1.5,
		remainingHeight / 2 - labelHeight - margin * 2);

	remainingHeight -= minFrequencyKnobRect.getHeight() + margin;

	minFrequencyKnobLabelRect = Rectangle<int>(
		minFrequencyKnobRect.getX(),
		minFrequencyKnobRect.getY() + minFrequencyKnobRect.getHeight() + margin,
		minFrequencyKnobRect.getWidth(),
		labelHeight);

	remainingHeight -= minFrequencyKnobLabelRect.getHeight() + margin * 2;

	sweepWidthKnobRect = Rectangle<int>(
		minFrequencyKnobRect.getX() + minFrequencyKnobRect.getWidth() + margin,
		minFrequencyKnobRect.getY(),
		minFrequencyKnobRect.getWidth(),
		minFrequencyKnobRect.getHeight());

	sweepWidthKnobLabelRect = Rectangle<int>(
		minFrequencyKnobLabelRect.getX() + minFrequencyKnobLabelRect.getWidth() + margin,
		minFrequencyKnobLabelRect.getY(),
		minFrequencyKnobLabelRect.getWidth(),
		minFrequencyKnobLabelRect.getHeight());

	feedbackKnobRect = Rectangle<int>(
		sweepWidthKnobRect.getX() + sweepWidthKnobRect.getWidth() + margin,
		sweepWidthKnobRect.getY(),
		sweepWidthKnobRect.getWidth(),
		sweepWidthKnobRect.getHeight());

	feedbackKnobLabelRect = Rectangle<int>(
		sweepWidthKnobLabelRect.getX() + sweepWidthKnobLabelRect.getWidth() + margin,
		sweepWidthKnobLabelRect.getY(),
		sweepWidthKnobLabelRect.getWidth(),
		sweepWidthKnobLabelRect.getHeight());

	depthKnobRect = Rectangle<int>(
		controlsRect.getX() + margin,
		controlsRect.getY() + controlsRect.getHeight() - remainingHeight,
		controlsRect.getWidth() / 2 - margin * 1.5,
		remainingHeight - (labelHeight + margin * 2));

	depthKnobLabelRect = Rectangle<int>(
		depthKnobRect.getX(),
		depthKnobRect.getY() + depthKnobRect.getHeight() + margin,
		depthKnobRect.getWidth(),
		labelHeight);

	dryWetKnobRect = Rectangle<int>(
		depthKnobRect.getX() + depthKnobRect.getWidth() + margin,
		depthKnobRect.getY(),
		depthKnobRect.getWidth(),
		depthKnobRect.getHeight());

	dryWetKnobLabelRect = Rectangle<int>(
		depthKnobLabelRect.getX() + depthKnobLabelRect.getWidth() + margin,
		depthKnobLabelRect.getY(),
		depthKnobLabelRect.getWidth(),
		depthKnobLabelRect.getHeight());
}

void PhaserAudioProcessorEditor::defineComponents()
{
}

//==============================================================================
