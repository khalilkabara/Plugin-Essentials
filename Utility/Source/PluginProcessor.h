/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class UtilityAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    UtilityAudioProcessor();
    ~UtilityAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	AudioProcessorValueTreeState valueTreeState;
	UndoManager undoManager;
	
	const String SAVED_PARAMS_NAME{ "worqUtilitySavedParams" };

	const String panParamName{ "pan" };
	const String gainParamName{ "gain" };
	const String muteParamName{ "mute" };

	const String allowedFileFormat = "*.wut";
	const String presetExtension = ".wut";
	
	void savePreset()
	{
		const auto state = valueTreeState.copyState();
		const std::unique_ptr<XmlElement> xml(state.createXml());

		FileChooser fileChooser{ "Save Preset", File() , allowedFileFormat };
		String url = "";

		if (fileChooser.browseForFileToSave(true))
			url = fileChooser.getResult().getFullPathName();
		else return;

		xml->writeTo(File(url));
	}

	void loadPreset()
	{
		File preset;
		FileChooser fileChooser{ "Load Preset", File(), allowedFileFormat };

		if (fileChooser.browseForFileToOpen())
			preset = fileChooser.getResult();
		else return;

		const std::unique_ptr<XmlElement> xml(parseXML(preset));

		valueTreeState.replaceState(ValueTree::fromXml(*xml));
	}

	void resetPlugin()
	{
		const ValueTree state = valueTreeState.copyState();
		const std::unique_ptr<XmlElement> tempXml(state.createXml());

		forEachXmlChildElementWithTagName(*tempXml, child, "PARAM")
		{
			const float defaultValue = valueTreeState.getParameter(child->getStringAttribute("id"))->getDefaultValue();
			const auto range = valueTreeState.getParameter(child->getStringAttribute("id"))->getNormalisableRange();
			const auto unRangedValue = jmap(defaultValue, range.start, range.end);
			child->setAttribute("value", unRangedValue);
		}

		valueTreeState.replaceState(ValueTree::fromXml(*tempXml));
	}

private:

	dsp::Panner<float> panner;
	
	void createStateTrees();

	void initDspEffects(dsp::ProcessSpec spec)
	{
		panner.reset();
		panner.prepare(spec);
		panner.setRule(dsp::PannerRule::balanced);
	}

	void updateDspParams()
	{
		panner.setPan(static_cast<float>(*valueTreeState.getRawParameterValue(panParamName)));
	}
	
	void processDspEffects(dsp::ProcessContextReplacing<float> dspContext)
	{
		updateDspParams();
		panner.process(dspContext);
	}
	
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UtilityAudioProcessor)
};
