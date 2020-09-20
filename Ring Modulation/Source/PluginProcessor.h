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

class RingModulationAudioProcessor : public AudioProcessor
{
public:
    //==============================================================================

    RingModulationAudioProcessor();
    ~RingModulationAudioProcessor();

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

    StringArray waveformItemsUI = {
        "Sine",
        "Triangle",
        "Saw Up",
        "Saw Down",
        "Square",
        "Square With Sloped Edges"
    };

    enum waveformIndex {
        waveformSine = 0,
        waveformTriangle,
        waveformSawtooth,
        waveformInverseSawtooth,
        waveformSquare,
        waveformSquareSlopedEdges,
    };

    //======================================

    float lfoPhase;
    float inverseSampleRate;
    float twoPi;

    float lfo (float phase, int waveform);

    //======================================

    PluginParametersManager parameters;

    PluginParameterLinSlider paramGain;
    PluginParameterLinSlider paramDepth;
    PluginParameterLinSlider paramFrequency;
    PluginParameterComboBox paramWaveform;
	
	const String gainParamName{ "gain" };
	const String depthParamName{ "depth" };
	const String frequencyParamName{ "frequency" };
	const String waveformParamName{ "wave" };

	const String allowedFileFormat = "*.wrm";
	const String presetExtension = ".wrm";

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RingModulationAudioProcessor)
};
