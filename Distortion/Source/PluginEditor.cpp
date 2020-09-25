/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

DistortionAudioProcessorEditor::DistortionAudioProcessorEditor(DistortionAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p)
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

				if (parameter->name == processor.inputGainParamName || 
					parameter->name == processor.outputGainParamName ||
					parameter->name == processor.toneParamName)
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

	backgroundImage = ImageCache::getFromMemory(BinaryData::distortionBackground_png,
	                                            BinaryData::distortionBackground_pngSize);

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

DistortionAudioProcessorEditor::~DistortionAudioProcessorEditor()
{
}

//==============================================================================

void DistortionAudioProcessorEditor::paint(Graphics& g)
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

		g.drawRect(inputGainKnobRect);
		g.drawRect(inputGainKnobLabelRect);
		g.drawRect(distortionTypeSelectorRect);
		g.drawRect(outputGainKnobLabelRect);
		g.drawRect(outputGainKnobRect);
		g.drawRect(distortionTypeLabelRect);
		g.drawRect(toneKnobRect);
		g.drawRect(toneKnobLabelRect);
	}
	//--------------------><

	g.drawFittedText("Distortion Type", distortionTypeLabelRect, Justification::centred, 1);
	g.drawFittedText("Input Gain", inputGainKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Tone", toneKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Output Gain", outputGainKnobLabelRect, Justification::centred, 1);
}

void DistortionAudioProcessorEditor::resized()
{
	defineRects();
	for (auto i = 0; i < components.size(); ++i)
	{
		if (components[i]->getName() == processor.distortionTypeParamName) 
			components[i]->setBounds(distortionTypeSelectorRect);
		if (components[i]->getName() == processor.inputGainParamName) components[i]->setBounds(inputGainKnobRect);
		if (components[i]->getName() == processor.outputGainParamName) components[i]->setBounds(outputGainKnobRect);
		if (components[i]->getName() == processor.toneParamName) components[i]->setBounds(toneKnobRect);
	}
	displayComponent.setBounds(headerDisplayRect);
	savePresetButton.setBounds(savePresetButtonRect);
	loadPresetButton.setBounds(loadPresetButtonRect);
}

void DistortionAudioProcessorEditor::defineRects()
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

	headerLeftRect = Rectangle<int>(
		headerRect.getX(),
		headerRect.getY() + margin / 2,
		headerRect.getWidth() * 2 / 5,
		headerRect.getHeight() - margin);

	headerDisplayRect = Rectangle<int>(
		headerLeftRect.getX() + headerLeftRect.getWidth(),
		headerLeftRect.getY(),
		headerLeftRect.getWidth() - border,
		headerLeftRect.getHeight());

	headerRightRect = Rectangle<int>(
		headerDisplayRect.getX() + headerDisplayRect.getWidth(),
		headerDisplayRect.getY(),
		headerDisplayRect.getWidth(),
		headerDisplayRect.getHeight());

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

	aboutButtonRect = Rectangle<int>(
		headerDisplayRect.getX() + headerDisplayRect.getWidth() + border,
		headerDisplayRect.getY(),
		headerRect.getWidth() * 1 / 5,
		headerDisplayRect.getHeight());

	//**************************************

	distortionTypeLabelRect = Rectangle<int>(
		mainArea.getX(),
		mainArea.getHeight() - remainingHeight,
		mainArea.getWidth() / 3,
		mainArea.getHeight() / 6);

	distortionTypeSelectorRect = Rectangle<int>(
		distortionTypeLabelRect.getX() + distortionTypeLabelRect.getWidth() + margin*2,
		distortionTypeLabelRect.getY(),
		mainArea.getWidth() * 2 / 3 - margin - 1,
		distortionTypeLabelRect.getHeight());

	remainingHeight -= distortionTypeSelectorRect.getHeight() + margin;

	inputGainKnobRect = Rectangle<int>(
		mainArea.getX(),
		mainArea.getHeight() - remainingHeight,
		mainArea.getWidth() / numKnobs,
		remainingHeight - labelHeight - margin);

	inputGainKnobLabelRect = Rectangle<int>(
		inputGainKnobRect.getX(),
		inputGainKnobRect.getY() + inputGainKnobRect.getHeight() + margin,
		inputGainKnobRect.getWidth(),
		labelHeight);

	toneKnobRect = Rectangle<int>(
		inputGainKnobRect.getX() + inputGainKnobRect.getWidth(),
		inputGainKnobRect.getY(),
		inputGainKnobRect.getWidth(),
		inputGainKnobRect.getHeight());

	toneKnobLabelRect = Rectangle<int>(
		inputGainKnobLabelRect.getX() + inputGainKnobLabelRect.getWidth(),
		inputGainKnobLabelRect.getY(),
		inputGainKnobLabelRect.getWidth(),
		inputGainKnobLabelRect.getHeight());

	outputGainKnobRect = Rectangle<int>(
		toneKnobRect.getX() + toneKnobRect.getWidth(),
		toneKnobRect.getY(),
		toneKnobRect.getWidth(),
		toneKnobRect.getHeight());

	outputGainKnobLabelRect = Rectangle<int>(
		toneKnobLabelRect.getX() + toneKnobLabelRect.getWidth(),
		toneKnobLabelRect.getY(),
		toneKnobLabelRect.getWidth(),
		toneKnobLabelRect.getHeight());
}

//==============================================================================
