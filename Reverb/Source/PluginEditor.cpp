/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverbAudioProcessorEditor::ReverbAudioProcessorEditor (ReverbAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
	setSize(pluginWidth + border * 2, pluginHeight + border);
	setLookAndFeel(&customGraphics);

	defineRects();
	defineComponents();

	backgroundImage = ImageCache::getFromMemory(BinaryData::reverbBackground_png,
		BinaryData::reverbBackground_pngSize);

	bypassToggleSliderAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTreeState,
		processor.bypassParamName,
		bypassToggleSlider));

	dampingKnobAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTreeState,
		processor.dampingParamName,
		dampingKnob));

	roomSizeKnobAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTreeState,
		processor.roomSizeParamName,
		roomSizeKnob));

	widthKnobAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTreeState,
		processor.widthParamName,
		widthKnob));

	dryLevelKnobAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTreeState,
		processor.dryLevelParamName,
		dryLevelKnob));

	wetLevelKnobAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTreeState,
		processor.wetLevelParamName,
		wetLevelKnob));

	gainKnobAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTreeState,
		processor.gainParamName,
		gainKnob));

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

ReverbAudioProcessorEditor::~ReverbAudioProcessorEditor()
{
}

//==============================================================================
void ReverbAudioProcessorEditor::paint (juce::Graphics& g)
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

		g.drawRect(bypassToggleRect);
		g.drawRect(bypassToggleLabelRect);
		
		g.drawRect(dampingKnobRect);
		g.drawRect(dampingKnobLabelRect);
		g.drawRect(roomSizeKnobRect);
		g.drawRect(roomSizeKnobLabelRect);
		g.drawRect(widthKnobRect);
		g.drawRect(widthKnobLabelRect);
		g.drawRect(dryLevelKnobRect);
		g.drawRect(dryLevelKnobLabelRect);
		g.drawRect(wetLevelKnobRect);
		g.drawRect(wetLevelKnobLabelRect);
		g.drawRect(gainKnobRect);
		g.drawRect(gainKnobLabelRect);
	}

	//--------------------><

	g.drawFittedText("Bypass", bypassToggleLabelRect, Justification::centred, 1);
	g.drawFittedText("Damping", dampingKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Reverb Size", roomSizeKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Width", widthKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Dry Level", dryLevelKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Wet Level", wetLevelKnobLabelRect, Justification::centred, 1);
	g.drawFittedText("Output Gain", gainKnobLabelRect, Justification::centred, 1);
}

void ReverbAudioProcessorEditor::resized()
{
	dampingKnob.setBounds(dampingKnobRect);
	gainKnob.setBounds(gainKnobRect);
	bypassToggle.setBounds(bypassToggleRect);
}

void ReverbAudioProcessorEditor::defineRects()
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

	bypassToggleLabelRect = Rectangle<int>(
		headerRightRect.getX() + margin,
		headerRightRect.getY(),
		headerRightRect.getWidth() / 4,
		headerRightRect.getHeight());

	bypassToggleRect = Rectangle<int>(
		bypassToggleLabelRect.getX() + bypassToggleLabelRect.getWidth() + margin,
		bypassToggleLabelRect.getY(),
		bypassToggleLabelRect.getWidth(),
		bypassToggleLabelRect.getHeight());

	aboutButtonRect = Rectangle<int>(
		bypassToggleRect.getX() + bypassToggleRect.getWidth() + margin,
		bypassToggleRect.getY(),
		headerRightRect.getWidth() / 2 - margin * 3,
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

	dampingKnobRect = Rectangle<int>(
		mainArea.getX(),
		mainArea.getY() + mainArea.getHeight() - remainingHeight,
		mainArea.getWidth() / 3,
		remainingHeight / 2 - labelHeight - margin);

	dampingKnobLabelRect = Rectangle<int>(
		dampingKnobRect.getX(),
		dampingKnobRect.getY() + dampingKnobRect.getHeight(),
		dampingKnobRect.getWidth(),
		labelHeight);

	roomSizeKnobRect = Rectangle<int>(
		dampingKnobRect.getX() + dampingKnobRect.getWidth(),
		dampingKnobRect.getY(),
		dampingKnobRect.getWidth(),
		dampingKnobRect.getHeight());

	roomSizeKnobLabelRect = Rectangle<int>(
		dampingKnobLabelRect.getX() + dampingKnobLabelRect.getWidth(),
		dampingKnobLabelRect.getY(),
		dampingKnobLabelRect.getWidth(),
		dampingKnobLabelRect.getHeight());

	widthKnobRect = Rectangle<int>(
		roomSizeKnobRect.getX() + roomSizeKnobRect.getWidth(),
		roomSizeKnobRect.getY(),
		roomSizeKnobRect.getWidth(),
		roomSizeKnobRect.getHeight());

	widthKnobLabelRect = Rectangle<int>(
		roomSizeKnobLabelRect.getX() + roomSizeKnobLabelRect.getWidth(),
		roomSizeKnobLabelRect.getY(),
		roomSizeKnobLabelRect.getWidth(),
		roomSizeKnobLabelRect.getHeight());

	dryLevelKnobRect = Rectangle<int>(
		dampingKnobLabelRect.getX(),
		dampingKnobLabelRect.getY() + dampingKnobLabelRect.getHeight() + margin,
		dampingKnobRect.getWidth(),
		dampingKnobRect.getHeight());

	dryLevelKnobLabelRect = Rectangle<int>(
		dryLevelKnobRect.getX(),
		dryLevelKnobRect.getY() + dryLevelKnobRect.getHeight(),
		dryLevelKnobRect.getWidth(),
		labelHeight);

	wetLevelKnobRect = Rectangle<int>(
		dryLevelKnobRect.getX() + dryLevelKnobRect.getWidth(),
		dryLevelKnobRect.getY(),
		dryLevelKnobRect.getWidth(),
		dryLevelKnobRect.getHeight());

	wetLevelKnobLabelRect = Rectangle<int>(
		dryLevelKnobLabelRect.getX() + dryLevelKnobLabelRect.getWidth(),
		dryLevelKnobLabelRect.getY(),
		dryLevelKnobLabelRect.getWidth(),
		dryLevelKnobLabelRect.getHeight());

	gainKnobRect = Rectangle<int>(
		wetLevelKnobRect.getX() + wetLevelKnobRect.getWidth(),
		wetLevelKnobRect.getY(),
		wetLevelKnobRect.getWidth(),
		wetLevelKnobRect.getHeight());

	gainKnobLabelRect = Rectangle<int>(
		wetLevelKnobLabelRect.getX() + wetLevelKnobLabelRect.getWidth(),
		wetLevelKnobLabelRect.getY(),
		wetLevelKnobLabelRect.getWidth(),
		wetLevelKnobLabelRect.getHeight());

}

void ReverbAudioProcessorEditor::defineComponents()
{
	bypassToggleSlider.setRange(0, 1, 1);
	bypassToggleSlider.setName(processor.bypassParamName);
	bypassToggleSlider.setVisible(false);
	bypassToggleSlider.addListener(this);

	bypassToggle.setBounds(bypassToggleRect);
	const auto isBypassed = static_cast<int>(*processor.valueTreeState.getRawParameterValue(
		processor.bypassParamName)) == 1;
	bypassToggle.setToggleState(isBypassed, NotificationType::dontSendNotification);
	bypassToggle.setVisible(true);
	bypassToggle.addListener(this);
	addAndMakeVisible(bypassToggle);

	dampingKnob.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	dampingKnob.setRange(0.0f, 1.0f, 0.01f);
	dampingKnob.setTextBoxStyle(Slider::NoTextBox, false, 100, 30);
	dampingKnob.setBounds(dampingKnobRect);
	dampingKnob.setSkewFactorFromMidPoint(0.3f);
	dampingKnob.setName(processor.dryLevelParamName);
	dampingKnob.setVisible(true);
	dampingKnob.addListener(this);
	addAndMakeVisible(dampingKnob);
	
	roomSizeKnob.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	roomSizeKnob.setRange(0.0f, 1.0f, 0.01f);
	roomSizeKnob.setTextBoxStyle(Slider::NoTextBox, false, 100, 30);
	roomSizeKnob.setBounds(roomSizeKnobRect);
	roomSizeKnob.setSkewFactorFromMidPoint(0.3f);
	roomSizeKnob.setName(processor.roomSizeParamName);
	roomSizeKnob.setVisible(true);
	roomSizeKnob.addListener(this);
	addAndMakeVisible(roomSizeKnob);
	
	widthKnob.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	widthKnob.setRange(0.0f, 1.0f, 0.01f);
	widthKnob.setTextBoxStyle(Slider::NoTextBox, false, 100, 30);
	widthKnob.setBounds(widthKnobRect);
	widthKnob.setSkewFactorFromMidPoint(0.3f);
	widthKnob.setName(processor.widthParamName);
	widthKnob.setVisible(true);
	widthKnob.addListener(this);
	addAndMakeVisible(widthKnob);
	
	dryLevelKnob.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	dryLevelKnob.setRange(0.0f, 1.0f, 0.01f);
	dryLevelKnob.setTextBoxStyle(Slider::NoTextBox, false, 100, 30);
	dryLevelKnob.setBounds(dryLevelKnobRect);
	dryLevelKnob.setSkewFactorFromMidPoint(0.3f);
	dryLevelKnob.setName(processor.dryLevelParamName);
	dryLevelKnob.setVisible(true);
	dryLevelKnob.addListener(this);
	addAndMakeVisible(dryLevelKnob);
	
	wetLevelKnob.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	wetLevelKnob.setRange(0.0f, 1.0f, 0.01f);
	wetLevelKnob.setTextBoxStyle(Slider::NoTextBox, false, 100, 30);
	wetLevelKnob.setBounds(wetLevelKnobRect);
	wetLevelKnob.setSkewFactorFromMidPoint(0.3f);
	wetLevelKnob.setName(processor.wetLevelParamName);
	wetLevelKnob.setVisible(true);
	wetLevelKnob.addListener(this);
	addAndMakeVisible(wetLevelKnob);
	
	gainKnob.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	gainKnob.setRange(-24.0f, 24.0f, 0.01f);
	gainKnob.setTextBoxStyle(Slider::NoTextBox, false, 100, 30);
	gainKnob.setBounds(gainKnobRect);
	// gainKnob.setSkewFactorFromMidPoint(0.3);
	gainKnob.setTooltip(translate("bipolar;"));
	gainKnob.setName(processor.gainParamName);
	gainKnob.setVisible(true);
	gainKnob.addListener(this);
	addAndMakeVisible(gainKnob);
	
}