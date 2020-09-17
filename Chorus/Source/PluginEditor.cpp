/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/


#include "PluginProcessor.h"
#include "PluginEditor.h"

ChorusAudioProcessorEditor::ChorusAudioProcessorEditor(ChorusAudioProcessor& p)
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

	backgroundImage = ImageCache::getFromMemory(BinaryData::chorusBackground__png,
		BinaryData::chorusBackground__pngSize);

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
}

ChorusAudioProcessorEditor::~ChorusAudioProcessorEditor()
{
}

//==============================================================================

void ChorusAudioProcessorEditor::paint(Graphics& g)
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
		g.drawRect(topSelectorsRect);
		g.drawRect(lfoRect);
		g.drawRect(controlsRect);
		g.drawRect(footerRect);
		//--------------------><
		g.drawRect(interpolationSelectorRect);
		g.drawRect(interpolationLabelRect);
		g.drawRect(numVoicesLabelRect);
		g.drawRect(numVoicesSelectorRect);

		g.drawRect(lfoTitleRect);
		g.drawRect(lfoWaveSelectorTitleRect);
		g.drawRect(lfoWaveSelectorRect);
		g.drawRect(lfoFrequencyKnobRect);
		g.drawRect(lfoFrequencyKnobLabelRect);

		g.drawRect(delayKnobRect);
		g.drawRect(delayKnobLabelRect);
		g.drawRect(widthKnobRect);
		g.drawRect(widthKnobLabelRect);
		g.drawRect(depthKnobRect);
		g.drawRect(depthKnobLabelRect);
		g.drawRect(gainKnobRect);
		g.drawRect(gainKnobLabelRect);
	}
	//--------------------><

	g.drawFittedText("Stereo", useStereoToggleLabelRect, Justification::centred, 1);
	g.drawFittedText("Num Voices", numVoicesLabelRect, Justification::centred, 1);
	g.drawFittedText("Interpolation", interpolationLabelRect, Justification::centred, 1);
	g.drawFittedText("LFO", lfoTitleRect, Justification::centred, 1);
	g.drawFittedText("Wave", lfoWaveSelectorTitleRect, Justification::centred, 1);
	g.drawFittedText("LFO frequency", lfoFrequencyKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Delay", delayKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Width", widthKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Depth", depthKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Gain", gainKnobLabelRect, Justification::centred, 1);
}

void ChorusAudioProcessorEditor::resized()
{
	defineRects();
	for (auto i = 0; i < components.size(); ++i)
	{
		if (components[i]->getName() == processor.interpolationParamName) components[i]->setBounds(interpolationSelectorRect);
		if (components[i]->getName() == processor.lfoWaveformParamName) components[i]->setBounds(lfoWaveSelectorRect);
		if (components[i]->getName() == processor.enableStereoParamName) components[i]->setBounds(useStereoToggleRect);
		if (components[i]->getName() == processor.numVoicesParamName) components[i]->setBounds(numVoicesSelectorRect);
		if (components[i]->getName() == processor.depthParamName) components[i]->setBounds(depthKnobRect);
		if (components[i]->getName() == processor.gainParamName) components[i]->setBounds(gainKnobRect);
		if (components[i]->getName() == processor.delayParamName) components[i]->setBounds(delayKnobRect);
		if (components[i]->getName() == processor.widthParamName) components[i]->setBounds(widthKnobRect);
		if (components[i]->getName() == processor.lfoFrequencyParamName) components[i]->setBounds(lfoFrequencyKnobRect);

	}
	displayComponent.setBounds(headerDisplayRect);
	savePresetButton.setBounds(savePresetButtonRect);
	loadPresetButton.setBounds(loadPresetButtonRect);
}

void ChorusAudioProcessorEditor::defineRects()
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

	topSelectorsRect = Rectangle<int>(
		headerRect.getX(),
		headerRect.getY() + headerRect.getHeight() + margin,
		headerRect.getWidth(),
		remainingHeight / 4);

	remainingHeight -= topSelectorsRect.getHeight() + margin;

	lfoRect = Rectangle<int>(
		mainArea.getX(),
		topSelectorsRect.getY() + topSelectorsRect.getHeight() + margin,
		mainArea.getWidth() / 3,
		remainingHeight);

	remainingHeight -= lfoRect.getHeight() + margin;

	controlsRect = Rectangle<int>(
		lfoRect.getX() + lfoRect.getWidth() + margin,
		lfoRect.getY(),
		mainArea.getWidth() * 2 / 3 - margin,
		lfoRect.getHeight());

	headerLeftRect = Rectangle<int>(
		headerRect.getX(),
		headerRect.getY() + margin / 2,
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
		headerRightRect.getX() + border,
		headerRightRect.getY(),
		headerRightRect.getWidth() * 2 / 6,
		headerRightRect.getHeight());

	useStereoToggleRect = Rectangle<int>(
		useStereoToggleLabelRect.getX() + useStereoToggleLabelRect.getWidth(),
		useStereoToggleLabelRect.getY(),
		headerRightRect.getWidth() * 1 / 6,
		useStereoToggleLabelRect.getHeight());

	loadPresetButtonRect = Rectangle<int>(
		headerLeftRect.getX(),
		headerLeftRect.getY(),
		headerLeftRect.getWidth() / 2 - border,
		headerLeftRect.getHeight());

	savePresetButtonRect = Rectangle<int>(
		loadPresetButtonRect.getX() + loadPresetButtonRect.getWidth() + border,
		loadPresetButtonRect.getY(),
		loadPresetButtonRect.getWidth(),
		loadPresetButtonRect.getHeight());


	// ***************************************************************************************

	interpolationLabelRect = Rectangle<int>(
		topSelectorsRect.getX(),
		topSelectorsRect.getY(),
		topSelectorsRect.getWidth() / 3,
		topSelectorsRect.getHeight() - topSelectorsRect.getHeight() / 2 - border / 2);

	interpolationSelectorRect = Rectangle<int>(
		interpolationLabelRect.getX() + interpolationLabelRect.getWidth(),
		interpolationLabelRect.getY(),
		topSelectorsRect.getWidth() * 2 / 3,
		interpolationLabelRect.getHeight());

	numVoicesLabelRect = Rectangle<int>(
		interpolationLabelRect.getX(),
		interpolationLabelRect.getY() + interpolationLabelRect.getHeight() + border/2,
		interpolationLabelRect.getWidth(),
		interpolationLabelRect.getHeight());

	numVoicesSelectorRect = Rectangle<int>(
		interpolationSelectorRect.getX(),
		interpolationSelectorRect.getY() + interpolationSelectorRect.getHeight() + border/2,
		interpolationSelectorRect.getWidth(),
		interpolationSelectorRect.getHeight());

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

	delayKnobRect = Rectangle<int>(
		controlsRect.getX() + margin,
		controlsRect.getY() + margin,
		controlsRect.getWidth() / 2 - margin * 1.5,
		remainingHeight / 2 - labelHeight - margin * 2);

	remainingHeight -= delayKnobRect.getHeight() + margin;

	delayKnobLabelRect = Rectangle<int>(
		delayKnobRect.getX(),
		delayKnobRect.getY() + delayKnobRect.getHeight() + margin,
		delayKnobRect.getWidth(),
		labelHeight);

	remainingHeight -= delayKnobLabelRect.getHeight() + margin * 2;

	widthKnobRect = Rectangle<int>(
		delayKnobRect.getX() + delayKnobRect.getWidth() + margin,
		delayKnobRect.getY(),
		delayKnobRect.getWidth(),
		delayKnobRect.getHeight());

	widthKnobLabelRect = Rectangle<int>(
		delayKnobLabelRect.getX() + delayKnobLabelRect.getWidth() + margin,
		delayKnobLabelRect.getY(),
		delayKnobLabelRect.getWidth(),
		delayKnobLabelRect.getHeight());

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
