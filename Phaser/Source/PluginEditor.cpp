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
	addComponents();
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
	g.drawFittedText("Enable Stereo", useStereoToggleLabelRect, Justification::centred, 1);
	
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
	useStereoToggle.setBounds(useStereoToggleRect);
	numFiltersSelector.setBounds(numFiltersSelectorRect);
	lfoWaveSelector.setBounds(lfoWaveSelectorRect);
	depthKnob.setBounds(depthKnobRect);
	feedbackKnob.setBounds(feedbackKnobRect);
	minFrequencyKnob.setBounds(minFrequencyKnobRect);
	sweepWidthKnob.setBounds(sweepWidthKnobRect);
	lfoFrequencyKnob.setBounds(lfoFrequencyKnobRect);
	dryWetKnob.setBounds(dryWetKnobRect);
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


	headerLeftRect = Rectangle<int> (
		headerRect.getX(),
		headerRect.getY(),
		headerRect.getWidth() / 3,
		headerRect.getHeight());
	
	headerDisplayRect = Rectangle<int> (
		headerLeftRect.getX() + headerLeftRect.getWidth(),
		headerLeftRect.getY(),
		headerLeftRect.getWidth(),
		headerLeftRect.getHeight());
	
	headerRightRect = Rectangle<int> (
		headerDisplayRect.getX() + headerDisplayRect.getWidth(),
		headerDisplayRect.getY(),
		headerDisplayRect.getWidth(),
		headerDisplayRect.getHeight());
	
	useStereoToggleLabelRect = Rectangle<int> (
		headerRightRect.getX(),
		headerRightRect.getY(),
		headerRightRect.getWidth() / 2,
		headerRightRect.getHeight());
	
	useStereoToggleRect = Rectangle<int> (
		useStereoToggleLabelRect.getX() + useStereoToggleLabelRect.getWidth(),
		useStereoToggleLabelRect.getY(),
		useStereoToggleLabelRect.getWidth(),
		useStereoToggleLabelRect.getHeight());
	
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
	useStereoToggleSlider.setRange(0, 1, 1);
	useStereoToggleSlider.setVisible(false);
	useStereoToggleSlider.addListener(this);

	const auto chorusIsEnabled = static_cast<int>(*processor.valueTreeState.getRawParameterValue(
		processor.useStereoParamName)) == 1;
	useStereoToggle.setToggleState(chorusIsEnabled, NotificationType::dontSendNotification);
	useStereoToggle.setVisible(true);
	useStereoToggle.addListener(this);

	const auto selectedIndex = static_cast<int>(*processor.valueTreeState.getRawParameterValue(processor.numFiltersParamName));
	numFiltersSelector.addItemList(processor.NUM_FILTERS, 1);
	numFiltersSelector.setText(processor.NUM_FILTERS[selectedIndex]);
	numFiltersSelector.setVisible(true);
	numFiltersSelector.setEnabled(true);
	numFiltersSelector.addListener(this);

	const auto selectedWave = static_cast<int>(*processor.valueTreeState.getRawParameterValue(processor.lfoWaveformParamName));
	lfoWaveSelector.addItemList(processor.LFO_WAVEFORMS, 1);
	lfoWaveSelector.setText(processor.LFO_WAVEFORMS[selectedWave]);
	lfoWaveSelector.setVisible(true);
	lfoWaveSelector.setEnabled(true);
	lfoWaveSelector.addListener(this);

	depthKnob.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	depthKnob.setRange(processor.zeroToOneMinValue, processor.zeroToOneMaxValue, processor.zeroToOneStepValue);
	depthKnob.setTextBoxStyle(Slider::NoTextBox, false, 100, 30);
	depthKnob.setSkewFactorFromMidPoint(processor.zeroToOneMidpointValue);
	depthKnob.setTooltip(translate("effect;"));
	depthKnob.setVisible(true);
	depthKnob.addListener(this);
	
	feedbackKnob.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	feedbackKnob.setRange(processor.zeroToOneMinValue, processor.zeroToOneMaxValue, processor.zeroToOneStepValue);
	feedbackKnob.setTextBoxStyle(Slider::NoTextBox, false, 100, 30);
	feedbackKnob.setSkewFactorFromMidPoint(processor.zeroToOneMidpointValue);
	feedbackKnob.setTooltip(translate("effect;"));
	feedbackKnob.setVisible(true);
	feedbackKnob.addListener(this);

	dryWetKnob.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	dryWetKnob.setRange(processor.zeroToOneMinValue, processor.zeroToOneMaxValue, processor.zeroToOneStepValue);
	dryWetKnob.setTextBoxStyle(Slider::NoTextBox, false, 100, 30);
	dryWetKnob.setSkewFactorFromMidPoint(processor.zeroToOneMidpointValue);
	dryWetKnob.setTooltip(translate("effect;"));
	dryWetKnob.setVisible(true);
	dryWetKnob.addListener(this);

	minFrequencyKnob.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	minFrequencyKnob.setRange(processor.minFrequencyMinValue, processor.minFrequencyMaxValue, processor.minFrequencyStepValue);
	minFrequencyKnob.setTextBoxStyle(Slider::NoTextBox, false, 100, 30);
	minFrequencyKnob.setSkewFactorFromMidPoint(processor.minFrequencyMidpointValue);
	minFrequencyKnob.setTooltip(translate("effect;"));
	minFrequencyKnob.setVisible(true);
	minFrequencyKnob.addListener(this);

	sweepWidthKnob.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	sweepWidthKnob.setRange(processor.sweepWidthMinValue, processor.sweepWidthMaxValue, processor.sweepWidthStepValue);
	sweepWidthKnob.setTextBoxStyle(Slider::NoTextBox, false, 100, 30);
	sweepWidthKnob.setSkewFactorFromMidPoint(processor.sweepWidthMidpointValue);
	sweepWidthKnob.setTooltip(translate("effect;"));
	sweepWidthKnob.setVisible(true);
	sweepWidthKnob.addListener(this);

	lfoFrequencyKnob.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	lfoFrequencyKnob.setRange(processor.lfoFrequencyMinValue, processor.lfoFrequencyMaxValue, processor.lfoFrequencyStepValue);
	lfoFrequencyKnob.setTextBoxStyle(Slider::NoTextBox, false, 100, 30);
	lfoFrequencyKnob.setSkewFactorFromMidPoint(processor.lfoFrequencyMidpointValue);
	lfoFrequencyKnob.setTooltip(translate("effect;"));
	lfoFrequencyKnob.setVisible(true);
	lfoFrequencyKnob.addListener(this);

	resized();
}

void PhaserAudioProcessorEditor::addComponents()
{
	addAndMakeVisible(useStereoToggle);
	addAndMakeVisible(numFiltersSelector);
	addAndMakeVisible(lfoWaveSelector);
	addAndMakeVisible(depthKnob);
	addAndMakeVisible(feedbackKnob);
	addAndMakeVisible(minFrequencyKnob);
	addAndMakeVisible(sweepWidthKnob);
	addAndMakeVisible(lfoFrequencyKnob);
	addAndMakeVisible(dryWetKnob);
}

//==============================================================================
