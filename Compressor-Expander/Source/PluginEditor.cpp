/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

CompressorExpanderAudioProcessorEditor::CompressorExpanderAudioProcessorEditor (CompressorExpanderAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
	const Array<AudioProcessorParameter*> parameters = processor.getParameters();
	int comboBoxCounter = 0;

	int editorHeight = 2 * editorMargin;
	for (int i = 0; i < parameters.size(); ++i)
	{
		if (const AudioProcessorParameterWithID* parameter =
			dynamic_cast<AudioProcessorParameterWithID*>(parameters[i]))
		{
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

				if (parameter->name == processor.makeupGainParamName)
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

			else if (processor.parameters.parameterTypes[i] == "ToggleButton")
			{
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

			else if (processor.parameters.parameterTypes[i] == "ComboBox")
			{
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

	backgroundImage = ImageCache::getFromMemory(BinaryData::compressorExpanderBackground_png,
		BinaryData::compressorExpanderBackground_pngSize);

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

CompressorExpanderAudioProcessorEditor::~CompressorExpanderAudioProcessorEditor()
{
}

//==============================================================================

void CompressorExpanderAudioProcessorEditor::paint(Graphics& g)
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

		g.drawRect(modeSelectorLabelRect);
		g.drawRect(modeSelectorRect);
		g.drawRect(thresholdKnobRect);
		g.drawRect(thresholdKnobLabelRect);
		g.drawRect(ratioKnobRect);
		g.drawRect(ratioKnobLabelRect);
		g.drawRect(attackKnobRect);
		g.drawRect(attackKnobLabelRect);
		g.drawRect(releaseKnobRect);
		g.drawRect(releaseKnobLabelRect);
		g.drawRect(makeupGainKnobRect);
		g.drawRect(makeupGainKnobLabelRect);
	}
	//--------------------><

	g.drawFittedText("Bypass", bypassToggleLabelRect, Justification::centred, 1);
	g.drawFittedText("Mode", modeSelectorLabelRect, Justification::centred, 1);
	g.drawFittedText("Threshold", thresholdKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Ratio", ratioKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Attack", attackKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Release", releaseKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Makeup Gain", makeupGainKnobLabelRect, Justification::centred, 1);
}

void CompressorExpanderAudioProcessorEditor::resized()
{

	defineRects();
	for (auto i = 0; i < components.size(); ++i)
	{
		if (components[i]->getName() == processor.bypassParamName) components[i]->setBounds(bypassToggleRect);
		if (components[i]->getName() == processor.modeParamName) components[i]->setBounds(modeSelectorRect);
		if (components[i]->getName() == processor.thresholdParamName) components[i]->setBounds(thresholdKnobRect);
		if (components[i]->getName() == processor.ratioParamName) components[i]->setBounds(ratioKnobRect);
		if (components[i]->getName() == processor.attackParamName) components[i]->setBounds(attackKnobRect);
		if (components[i]->getName() == processor.releaseParamName) components[i]->setBounds(releaseKnobRect);
		if (components[i]->getName() == processor.makeupGainParamName) components[i]->setBounds(makeupGainKnobRect);

	}
	displayComponent.setBounds(headerDisplayRect);
	savePresetButton.setBounds(savePresetButtonRect);
	loadPresetButton.setBounds(loadPresetButtonRect);
}

void CompressorExpanderAudioProcessorEditor::defineRects()
{
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
		remainingHeight / 5);

	remainingHeight -= topSelectorsRect.getHeight() + margin;

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

	bypassToggleLabelRect = Rectangle<int>(
		headerRightRect.getX() + margin,
		headerRightRect.getY(),
		headerRightRect.getWidth() * 2 / 6,
		headerRightRect.getHeight());

	bypassToggleRect = Rectangle<int>(
		bypassToggleLabelRect.getX() + bypassToggleLabelRect.getWidth() + margin,
		bypassToggleLabelRect.getY(),
		headerRightRect.getWidth() * 1 / 6,
		bypassToggleLabelRect.getHeight());

	aboutButtonRect = Rectangle<int>(
		bypassToggleRect.getX() + bypassToggleRect.getWidth() + margin,
		bypassToggleRect.getY(),
		headerRightRect.getWidth() * 2 / 6,
		bypassToggleRect.getHeight());

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


	modeSelectorLabelRect = Rectangle<int>(
		topSelectorsRect.getX(),
		topSelectorsRect.getY(),
		topSelectorsRect.getWidth() / 3,
		topSelectorsRect.getHeight());

	modeSelectorRect = Rectangle<int>(
		modeSelectorLabelRect.getX() + modeSelectorLabelRect.getWidth(),
		modeSelectorLabelRect.getY(),
		topSelectorsRect.getWidth() * 2 / 3,
		modeSelectorLabelRect.getHeight());

	thresholdKnobRect = Rectangle<int>(
		mainArea.getX(),
		mainArea.getY() + mainArea.getHeight() - remainingHeight,
		mainArea.getWidth() / numKnobs,
		remainingHeight - labelHeight - margin);

	thresholdKnobLabelRect = Rectangle<int>(
		thresholdKnobRect.getX(),
		thresholdKnobRect.getY() + thresholdKnobRect.getHeight() + margin,
		thresholdKnobRect.getWidth(),
		labelHeight);

	ratioKnobRect = Rectangle<int>(
		thresholdKnobRect.getX() + thresholdKnobRect.getWidth(),
		thresholdKnobRect.getY(),
		thresholdKnobRect.getWidth(),
		thresholdKnobRect.getHeight());

	ratioKnobLabelRect = Rectangle<int>(
		thresholdKnobLabelRect.getX() + thresholdKnobLabelRect.getWidth(),
		thresholdKnobLabelRect.getY(),
		thresholdKnobLabelRect.getWidth(),
		thresholdKnobLabelRect.getHeight());

	attackKnobRect = Rectangle<int>(
		ratioKnobRect.getX() + ratioKnobRect.getWidth(),
		ratioKnobRect.getY(),
		ratioKnobRect.getWidth(),
		ratioKnobRect.getHeight());

	attackKnobLabelRect = Rectangle<int>(
		ratioKnobLabelRect.getX() + ratioKnobLabelRect.getWidth(),
		ratioKnobLabelRect.getY(),
		ratioKnobLabelRect.getWidth(),
		ratioKnobLabelRect.getHeight());
	
	releaseKnobRect = Rectangle<int>(
		attackKnobRect.getX() + attackKnobRect.getWidth(),
		attackKnobRect.getY(),
		attackKnobRect.getWidth(),
		attackKnobRect.getHeight());

	releaseKnobLabelRect = Rectangle<int>(
		attackKnobLabelRect.getX() + attackKnobLabelRect.getWidth(),
		attackKnobLabelRect.getY(),
		attackKnobLabelRect.getWidth(),
		attackKnobLabelRect.getHeight());

	makeupGainKnobRect = Rectangle<int>(
		releaseKnobRect.getX() + releaseKnobRect.getWidth(),
		releaseKnobRect.getY(),
		releaseKnobRect.getWidth(),
		releaseKnobRect.getHeight());

	makeupGainKnobLabelRect = Rectangle<int>(
		releaseKnobLabelRect.getX() + releaseKnobLabelRect.getWidth(),
		releaseKnobLabelRect.getY(),
		releaseKnobLabelRect.getWidth(),
		releaseKnobLabelRect.getHeight());
}

//==============================================================================
