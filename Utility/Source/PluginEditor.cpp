/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
UtilityAudioProcessorEditor::UtilityAudioProcessorEditor (UtilityAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
	setSize(pluginWidth + border * 2, pluginHeight + border);
	setLookAndFeel(&customGraphics);

	defineRects();
	defineComponents();

	backgroundImage = ImageCache::getFromMemory(BinaryData::utilityBackground_png,
		BinaryData::utilityBackground_pngSize);


	panKnobAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTreeState,
		processor.panParamName,
		panKnob));

	gainKnobAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTreeState,
		processor.gainParamName,
		gainKnob));

	muteToggleSliderAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTreeState,
		processor.muteParamName,
		muteToggleSlider));

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

UtilityAudioProcessorEditor::~UtilityAudioProcessorEditor()
{
}

//==============================================================================
void UtilityAudioProcessorEditor::paint (juce::Graphics& g)
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
		//--------------------><

		g.drawRect(panKnobRect);
		g.drawRect(panKnobLabelRect);
		g.drawRect(gainKnobRect);
		g.drawRect(gainKnobLabelRect);
		g.drawRect(muteToggleRect);
		g.drawRect(muteToggleLabelRect);
	}

	//--------------------><

	g.drawFittedText("Pan", panKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Gain", gainKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Mute", muteToggleLabelRect, Justification::centred, 1);
}

void UtilityAudioProcessorEditor::resized()
{
	panKnob.setBounds(panKnobRect);
	gainKnob.setBounds(gainKnobRect);
	muteToggle.setBounds(muteToggleRect);
}

void UtilityAudioProcessorEditor::defineRects()
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

	muteToggleLabelRect = Rectangle<int>(
		headerRightRect.getX() + margin,
		headerRightRect.getY(),
		headerRightRect.getWidth() / 4,
		headerRightRect.getHeight());

	muteToggleRect = Rectangle<int>(
		muteToggleLabelRect.getX() + muteToggleLabelRect.getWidth() + margin,
		muteToggleLabelRect.getY(),
		muteToggleLabelRect.getWidth(),
		muteToggleLabelRect.getHeight());

	aboutButtonRect = Rectangle<int>(
		muteToggleRect.getX() + muteToggleRect.getWidth() + margin,
		muteToggleRect.getY(),
		headerRightRect.getWidth() / 2 - margin * 3,
		muteToggleRect.getHeight());

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

	panKnobRect = Rectangle<int>(
		mainArea.getX(),
		mainArea.getY() + mainArea.getHeight() - remainingHeight,
		mainArea.getWidth() / 2,
		remainingHeight - labelHeight - margin);

	panKnobLabelRect = Rectangle<int>(
		panKnobRect.getX(),
		panKnobRect.getY() + panKnobRect.getHeight() + margin,
		panKnobRect.getWidth(),
		labelHeight);

	gainKnobRect = Rectangle<int>(
		panKnobRect.getX() + panKnobRect.getWidth() + margin,
		panKnobRect.getY(),
		panKnobRect.getWidth(),
		panKnobRect.getHeight());

	gainKnobLabelRect = Rectangle<int>(
		panKnobLabelRect.getX() + panKnobLabelRect.getWidth() + margin,
		panKnobLabelRect.getY(),
		panKnobLabelRect.getWidth(),
		panKnobLabelRect.getHeight());

}

void UtilityAudioProcessorEditor::defineComponents()
{
	muteToggleSlider.setRange(0, 1, 1);
	muteToggleSlider.setName(processor.muteParamName);
	muteToggleSlider.setVisible(false);
	muteToggleSlider.addListener(this);

	muteToggle.setBounds(muteToggleRect);
	const auto isMuted = static_cast<int>(*processor.valueTreeState.getRawParameterValue(
		processor.muteParamName)) == 1;
	muteToggle.setToggleState(isMuted, NotificationType::dontSendNotification);
	muteToggle.setVisible(true);
	muteToggle.addListener(this);
	addAndMakeVisible(muteToggle);

	panKnob.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	panKnob.setRange(-1.0f, 1.0f, 0.01f);
	panKnob.setTextBoxStyle(Slider::NoTextBox, false, 100, 30);
	panKnob.setBounds(panKnobRect);
	// panKnob.setSkewFactorFromMidPoint(0.3);
	panKnob.setTooltip(translate("bipolar;"));
	panKnob.setName(processor.panParamName);
	panKnob.setVisible(true);
	panKnob.addListener(this);
	addAndMakeVisible(panKnob);
	
	gainKnob.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	gainKnob.setRange(-48.0f, 48.0f, 0.01f);
	gainKnob.setTextBoxStyle(Slider::NoTextBox, false, 100, 30);
	gainKnob.setBounds(gainKnobRect);
	// gainKnob.setSkewFactorFromMidPoint(0.3);
	gainKnob.setTooltip(translate("bipolar;"));
	gainKnob.setName(processor.gainParamName);
	gainKnob.setVisible(true);
	gainKnob.addListener(this);
	addAndMakeVisible(gainKnob);
	
}