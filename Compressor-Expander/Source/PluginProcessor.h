/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PluginParameter.h"

//==============================================================================

class CompressorExpanderAudioProcessor : public AudioProcessor
{
public:
    //==============================================================================

    CompressorExpanderAudioProcessor();
    ~CompressorExpanderAudioProcessor();

    //==============================================================================

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================

    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    //==============================================================================

    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect () const override;
    double getTailLengthSeconds() const override;

    //==============================================================================

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================

    AudioSampleBuffer mixedDownInput;
    float xl;
    float yl;
    float xg;
    float yg;
    float control;

    float inputLevel;
    float ylPrev;

    float inverseSampleRate;
    float inverseE;
    float calculateAttackOrRelease (float value);

    //======================================

    PluginParametersManager parameters;

    PluginParameterComboBox paramMode;
    PluginParameterLinSlider paramThreshold;
    PluginParameterLinSlider paramRatio;
    PluginParameterLinSlider paramAttack;
    PluginParameterLinSlider paramRelease;
    PluginParameterLinSlider paramMakeupGain;
    PluginParameterToggle paramBypass;
	
	const String modeParamName{ "mode" };
	const String thresholdParamName{ "threshold" };
	const String ratioParamName{ "ratio" };
	const String attackParamName{ "attack" };
	const String releaseParamName{ "release" };
	const String makeupGainParamName{ "makeupGain" };
	const String bypassParamName{ "bypass" };

	const String allowedFileFormat = "*.wce";
	const String presetExtension = ".wce";

	void savePreset()
	{
		const auto state = parameters.valueTreeState.copyState();
		const std::unique_ptr<XmlElement> xml(state.createXml());

		FileChooser fileChooser{ "Save Preset", File(), allowedFileFormat };
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

		parameters.valueTreeState.replaceState(ValueTree::fromXml(*xml));
	}

	void resetPlugin()
	{
		const ValueTree state = parameters.valueTreeState.copyState();
		const std::unique_ptr<XmlElement> tempXml(state.createXml());

		forEachXmlChildElementWithTagName(*tempXml, child, "PARAM")
		{
			const float defaultValue = parameters
				.valueTreeState.getParameter(child->getStringAttribute("id"))->getDefaultValue();
			const auto range = parameters.valueTreeState.getParameter(child->getStringAttribute("id"))->
				getNormalisableRange();
			const auto unRangedValue = jmap(defaultValue, range.start, range.end);
			child->setAttribute("value", unRangedValue);
		}

		parameters.valueTreeState.replaceState(ValueTree::fromXml(*tempXml));
	}


private:
    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompressorExpanderAudioProcessor)
};
