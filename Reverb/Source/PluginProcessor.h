/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

class ReverbAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ReverbAudioProcessor();
    ~ReverbAudioProcessor() override;

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
	
	const String SAVED_PARAMS_NAME{ "worqReverbSavedParams" };

	const String bypassParamName{ "bypass" };
	const String dryLevelParamName{ "dryLevel" };
	const String wetLevelParamName{ "wetLevel" };
	const String dampingParamName{ "damping" };
	const String roomSizeParamName{ "roomSize" };
	const String widthParamName{ "width" };
	const String gainParamName{ "gain" };

	const String allowedFileFormat = "*.wrv";
	const String presetExtension = ".wrv";
	
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

	dsp::Reverb reverb;
	dsp::Reverb::Parameters reverbParams;
	
	void createStateTrees();

	void initDspEffects(dsp::ProcessSpec spec)
	{
		reverb.reset();
		reverb.prepare(spec);
	}

	void updateDspParams()
	{
		reverbParams.damping = static_cast<float>(*valueTreeState.getRawParameterValue(dampingParamName));
		reverbParams.roomSize = static_cast<float>(*valueTreeState.getRawParameterValue(roomSizeParamName));
		reverbParams.width = static_cast<float>(*valueTreeState.getRawParameterValue(widthParamName));
		reverbParams.dryLevel = static_cast<float>(*valueTreeState.getRawParameterValue(dryLevelParamName));
		reverbParams.wetLevel = static_cast<float>(*valueTreeState.getRawParameterValue(wetLevelParamName));

		reverb.setEnabled(static_cast<float>(*valueTreeState.getRawParameterValue(bypassParamName)) == 0);
		reverb.setParameters(reverbParams);
	}
	
	void processDspEffects(dsp::ProcessContextReplacing<float> dspContext)
	{
		updateDspParams();
		reverb.process(dspContext);
	}
	
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbAudioProcessor)
};
