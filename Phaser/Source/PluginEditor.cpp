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
	const Array<AudioProcessorParameter*> parameters = processor.getParameters();
	int comboBoxCounter = 0;

	int editorHeight = 2 * editorMargin;
	for (int i = 0; i < parameters.size(); ++i) {
		if (const AudioProcessorParameterWithID* parameter =
			dynamic_cast<AudioProcessorParameterWithID*> (parameters[i])) {

			if (processor.parameters.parameterTypes[i] == "Slider") 
			{
				Slider* aSlider;
				sliders.add(aSlider = new Slider());
				aSlider->setTextValueSuffix(parameter->label);
				aSlider->setName(parameter->name);
				aSlider->setVisible(true);
				aSlider->addListener(this);
				aSlider->setSliderStyle(Slider::RotaryVerticalDrag);
				aSlider->setTextBoxStyle(Slider::NoTextBox,
					false, sliderTextEntryBoxWidth, sliderTextEntryBoxHeight);

				if (parameter->name == processor.gainParamName)
				{
					aSlider->setTooltip(translate("bipolar;"));
				}
				
				SliderAttachment* aSliderAttachment;
				sliderAttachments.add(aSliderAttachment =
					new SliderAttachment(processor.parameters.valueTreeState, parameter->paramID, *aSlider));

				components.add(aSlider);
				editorHeight += sliderHeight;
			}

			//======================================

			else if (processor.parameters.parameterTypes[i] == "ToggleButton") {
				ToggleButton* aButton;
				toggles.add(aButton = new ToggleButton());
				aButton->setName(parameter->name);
				aButton->addListener(this);
				aButton->setToggleState(parameter->getDefaultValue(), dontSendNotification);

				ButtonAttachment* aButtonAttachment;
				buttonAttachments.add(aButtonAttachment =
					new ButtonAttachment(processor.parameters.valueTreeState, parameter->paramID, *aButton));

				components.add(aButton);
				editorHeight += buttonHeight;
			}

			//======================================

			else if (processor.parameters.parameterTypes[i] == "ComboBox") {
				ComboBox* aComboBox;
				comboBoxes.add(aComboBox = new ComboBox());
				aComboBox->setEditableText(false);
				aComboBox->setName(parameter->name);
				aComboBox->addListener(this);
				aComboBox->setJustificationType(Justification::left);
				aComboBox->addItemList(processor.parameters.comboBoxItemLists[comboBoxCounter++], 1);

				ComboBoxAttachment* aComboBoxAttachment;
				comboBoxAttachments.add(aComboBoxAttachment =
					new ComboBoxAttachment(processor.parameters.valueTreeState, parameter->paramID, *aComboBox));

				components.add(aComboBox);
				editorHeight += comboBoxHeight;
			}

			components.getLast()->setName(parameter->name);
			components.getLast()->setComponentID(parameter->paramID);
			addAndMakeVisible(components.getLast());
		}
	}

	setSize(pluginWidth + border * 2, pluginHeight + border);
	setLookAndFeel(&customGraphics);

	defineRects();

	backgroundImage = ImageCache::getFromMemory(BinaryData::phaserBackground_png,
		BinaryData::phaserBackground_pngSize);
	 
	displayComponent.setVisible(true);
	displayComponent.setBounds(headerDisplayRect);
	addAndMakeVisible(displayComponent);

	savePresetButton.setButtonText("Save Preset");
	savePresetButton.setVisible(true);
	savePresetButton.changeWidthToFitText();
	savePresetButton.addListener(this);
	savePresetButton.setBounds(savePresetButtonRect);
	addAndMakeVisible(savePresetButton);

	loadPresetButton.setButtonText("Load Preset");
	loadPresetButton.setVisible(true);
	loadPresetButton.changeWidthToFitText();
	loadPresetButton.addListener(this);
	loadPresetButton.setBounds(loadPresetButtonRect);
	addAndMakeVisible(loadPresetButton);

	aboutButton.setButtonText("About");
	aboutButton.setVisible(true);
	aboutButton.changeWidthToFitText();
	aboutButton.addListener(this);
	aboutButton.setBounds(aboutButtonRect);
	addAndMakeVisible(aboutButton);
}

PhaserAudioProcessorEditor::~PhaserAudioProcessorEditor()
{
}

//==============================================================================

void PhaserAudioProcessorEditor::paint(Graphics& g)
{
	g.setColour(Colours::whitesmoke);
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

	g.drawImageWithin(*&backgroundImage,
		getLocalBounds().getX(),
		getLocalBounds().getY(),
		getLocalBounds().getWidth(),
		getLocalBounds().getHeight(), RectanglePlacement::fillDestination);

	if (displayComponent.debugRects)
	{
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
		g.drawRect(gainKnobRect);
		g.drawRect(gainKnobLabelRect);
	}
	
	//--------------------><
	
	g.drawFittedText("Stereo", useStereoToggleLabelRect, Justification::centred, 1);

	g.drawFittedText("Num Filters", numFiltersLabelRect, Justification::centred, 1);

	g.drawFittedText("LFO", lfoTitleRect, Justification::centred, 1);
	g.drawFittedText("Wave", lfoWaveSelectorTitleRect, Justification::centred, 1);
	g.drawFittedText("LFO frequency", lfoFrequencyKnobLabelRect, Justification::centred, 1);

	g.drawFittedText("Frequency", minFrequencyKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Width", sweepWidthKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Feedback", feedbackKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Depth", depthKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Gain", gainKnobLabelRect, Justification::centred, 1);
}

void PhaserAudioProcessorEditor::resized()
{
	defineRects();
	for (auto i = 0; i < components.size(); ++i)
	{
		if (components[i]->getName() == processor.numFiltersParamName) components[i]->setBounds(numFiltersSelectorRect);
		if (components[i]->getName() == processor.lfoWaveformParamName) components[i]->setBounds(lfoWaveSelectorRect);
		if (components[i]->getName() == processor.useStereoParamName) components[i]->setBounds(useStereoToggleRect);
		if (components[i]->getName() == processor.depthParamName) components[i]->setBounds(depthKnobRect);
		if (components[i]->getName() == processor.feedbackParamName) components[i]->setBounds(feedbackKnobRect);
		if (components[i]->getName() == processor.gainParamName) components[i]->setBounds(gainKnobRect);
		if (components[i]->getName() == processor.minFrequencyParamName) components[i]->setBounds(minFrequencyKnobRect);
		if (components[i]->getName() == processor.sweepWidthParamName) components[i]->setBounds(sweepWidthKnobRect);
		if (components[i]->getName() == processor.lfoFrequencyParamName) components[i]->setBounds(lfoFrequencyKnobRect);

	}
	displayComponent.setBounds(headerDisplayRect);
	savePresetButton.setBounds(savePresetButtonRect);
	loadPresetButton.setBounds(loadPresetButtonRect);
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
		remainingHeight);

	// aboutButtonRect = Rectangle<int>(
	// 	mainArea.getX() + mainArea.getWidth() - aboutButtonWidth,
	// 	lfoRect.getY() + lfoRect.getHeight() + margin,
	// 	aboutButtonWidth,
	// 	headerDisplayRect.getHeight());

	remainingHeight -= lfoRect.getHeight() + margin;

	controlsRect = Rectangle<int>(
		lfoRect.getX() + lfoRect.getWidth() + margin,
		lfoRect.getY(),
		mainArea.getWidth() * 2 / 3 - margin,
		lfoRect.getHeight());

	headerLeftRect = Rectangle<int>(
		headerRect.getX(),
		headerRect.getY() + margin/2,
		headerRect.getWidth() / 3,
		headerRect.getHeight() - margin);

	headerDisplayRect = Rectangle<int>(
		headerLeftRect.getX() + headerLeftRect.getWidth(),
		headerLeftRect.getY(),
		headerLeftRect.getWidth(),
		headerLeftRect.getHeight());

	headerRightRect = Rectangle<int>(
		headerDisplayRect.getX() + headerDisplayRect.getWidth(),
		headerDisplayRect.getY(),
		headerDisplayRect.getWidth(),
		headerDisplayRect.getHeight());

	useStereoToggleLabelRect = Rectangle<int>(
		headerRightRect.getX() + margin,
		headerRightRect.getY(),
		headerRightRect.getWidth() / 4,
		headerRightRect.getHeight());

	useStereoToggleRect = Rectangle<int>(
		useStereoToggleLabelRect.getX() + useStereoToggleLabelRect.getWidth() + margin,
		useStereoToggleLabelRect.getY(),
		useStereoToggleLabelRect.getWidth(),
		useStereoToggleLabelRect.getHeight());

	aboutButtonRect = Rectangle<int>(
		useStereoToggleRect.getX() + useStereoToggleRect.getWidth() + margin,
		useStereoToggleRect.getY(),
		headerRightRect.getWidth() / 2 - margin * 3,
		useStereoToggleRect.getHeight());

	loadPresetButtonRect = Rectangle<int>(
		headerLeftRect.getX(),
		headerLeftRect.getY(),
		headerLeftRect.getWidth() / 2 - margin,
		headerLeftRect.getHeight());

	savePresetButtonRect = Rectangle<int>(
		loadPresetButtonRect.getX() + loadPresetButtonRect.getWidth() + margin,
		loadPresetButtonRect.getY(),
		loadPresetButtonRect.getWidth(),
		loadPresetButtonRect.getHeight());

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

	gainKnobRect = Rectangle<int>(
		depthKnobRect.getX() + depthKnobRect.getWidth() + margin,
		depthKnobRect.getY(),
		depthKnobRect.getWidth(),
		depthKnobRect.getHeight());

	gainKnobLabelRect = Rectangle<int>(
		depthKnobLabelRect.getX() + depthKnobLabelRect.getWidth() + margin,
		depthKnobLabelRect.getY(),
		depthKnobLabelRect.getWidth(),
		depthKnobLabelRect.getHeight());
}
