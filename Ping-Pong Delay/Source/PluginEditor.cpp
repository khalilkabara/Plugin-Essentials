/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/


#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

PingPongDelayAudioProcessorEditor::PingPongDelayAudioProcessorEditor(PingPongDelayAudioProcessor& p)
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

				if (parameter->name == processor.gainParamName || parameter->name == processor.balanceParamName)
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

	backgroundImage = ImageCache::getFromMemory(BinaryData::pingpongBackground_png,
	                                            BinaryData::pingpongBackground_pngSize);

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

PingPongDelayAudioProcessorEditor::~PingPongDelayAudioProcessorEditor()
{
}

//==============================================================================

void PingPongDelayAudioProcessorEditor::paint(Graphics& g)
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

		g.drawRect(gainKnobRect);
		g.drawRect(gainKnobLabelRect);
		g.drawRect(feedbackKnobRect);
		g.drawRect(feedbackKnobLabelRect);
		g.drawRect(delayTimeKnobRect);
		g.drawRect(delayTimeKnobLabelRect);
		g.drawRect(mixKnobRect);
		g.drawRect(mixKnobLabelRect);
	}
	//--------------------><

	g.drawFittedText("Gain", gainKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Balance", balanceKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Feedback", feedbackKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Time", delayTimeKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Mix", mixKnobLabelRect, Justification::centred, 1);
}

void PingPongDelayAudioProcessorEditor::resized()
{
	defineRects();
	for (auto i = 0; i < components.size(); ++i)
	{
		if (components[i]->getName() == processor.feedbackParamName) components[i]->setBounds(feedbackKnobRect);
		if (components[i]->getName() == processor.balanceParamName) components[i]->setBounds(balanceKnobRect);
		if (components[i]->getName() == processor.gainParamName) components[i]->setBounds(gainKnobRect);
		if (components[i]->getName() == processor.delayTimeParamName) components[i]->setBounds(delayTimeKnobRect);
		if (components[i]->getName() == processor.mixParamName) components[i]->setBounds(mixKnobRect);
	}
	displayComponent.setBounds(headerDisplayRect);
	savePresetButton.setBounds(savePresetButtonRect);
	loadPresetButton.setBounds(loadPresetButtonRect);
}

void PingPongDelayAudioProcessorEditor::defineRects()
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

	delayTimeKnobRect = Rectangle<int>(
		mainArea.getX(),
		mainArea.getHeight() - remainingHeight,
		mainArea.getWidth() / numKnobs,
		remainingHeight - labelHeight - margin);

	delayTimeKnobLabelRect = Rectangle<int>(
		delayTimeKnobRect.getX(),
		delayTimeKnobRect.getY() + delayTimeKnobRect.getHeight() + margin,
		delayTimeKnobRect.getWidth(),
		labelHeight);

	feedbackKnobRect = Rectangle<int>(
		delayTimeKnobRect.getX() + delayTimeKnobRect.getWidth(),
		delayTimeKnobRect.getY(),
		delayTimeKnobRect.getWidth(),
		delayTimeKnobRect.getHeight());

	feedbackKnobLabelRect = Rectangle<int>(
		delayTimeKnobLabelRect.getX() + delayTimeKnobLabelRect.getWidth(),
		delayTimeKnobLabelRect.getY(),
		delayTimeKnobLabelRect.getWidth(),
		delayTimeKnobLabelRect.getHeight());

	balanceKnobRect = Rectangle<int>(
		feedbackKnobRect.getX() + feedbackKnobRect.getWidth(),
		feedbackKnobRect.getY(),
		feedbackKnobRect.getWidth(),
		feedbackKnobRect.getHeight());

	balanceKnobLabelRect = Rectangle<int>(
		feedbackKnobLabelRect.getX() + feedbackKnobLabelRect.getWidth(),
		feedbackKnobLabelRect.getY(),
		feedbackKnobLabelRect.getWidth(),
		feedbackKnobLabelRect.getHeight());

	mixKnobRect = Rectangle<int>(
		balanceKnobRect.getX() + balanceKnobRect.getWidth(),
		balanceKnobRect.getY(),
		balanceKnobRect.getWidth(),
		balanceKnobRect.getHeight());

	mixKnobLabelRect = Rectangle<int>(
		balanceKnobLabelRect.getX() + balanceKnobLabelRect.getWidth(),
		balanceKnobLabelRect.getY(),
		balanceKnobLabelRect.getWidth(),
		balanceKnobLabelRect.getHeight());

	gainKnobRect = Rectangle<int>(
		mixKnobRect.getX() + mixKnobRect.getWidth(),
		mixKnobRect.getY(),
		mixKnobRect.getWidth(),
		mixKnobRect.getHeight());

	gainKnobLabelRect = Rectangle<int>(
		mixKnobLabelRect.getX() + mixKnobLabelRect.getWidth(),
		mixKnobLabelRect.getY(),
		mixKnobLabelRect.getWidth(),
		mixKnobLabelRect.getHeight());
}

//==============================================================================
