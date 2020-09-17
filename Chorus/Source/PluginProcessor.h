/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/


#pragma once

#define _USE_MATH_DEFINES
#include <cmath>


#include "../../Common/PluginParameter.h"
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================

class ChorusAudioProcessor : public AudioProcessor
{
public:
    //==============================================================================

    ChorusAudioProcessor();
    ~ChorusAudioProcessor();

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
        "Saw Dn"
    };

    enum waveformIndex {
        waveformSine = 0,
        waveformTriangle,
        waveformSawtooth,
        waveformInverseSawtooth,
    };

    //======================================

    StringArray interpolationItemsUI = {
        "None",
        "Linear",
        "Cubic"
    };

    enum interpolationIndex {
        interpolationNearestNeighbour = 0,
        interpolationLinear,
        interpolationCubic,
    };

    //======================================

    AudioSampleBuffer delayBuffer;
    int delayBufferSamples;
    int delayBufferChannels;
    int delayWritePosition;

    float lfoPhase;
    float inverseSampleRate;
    float twoPi;

    float lfo (float phase, int waveform);

    //======================================

    PluginParametersManager parameters;

    PluginParameterLinSlider paramGain;
    PluginParameterLinSlider paramDelay;
    PluginParameterLinSlider paramWidth;
    PluginParameterLinSlider paramDepth;
    PluginParameterComboBox paramNumVoices;
    PluginParameterLinSlider paramFrequency;
    PluginParameterComboBox paramWaveform;
    PluginParameterComboBox paramInterpolation;
    PluginParameterToggle paramStereo;

	UndoManager undoManager;
	const String SAVED_PARAMS_NAME{ "savedParams" };

	const String gainParamName{ "gain" };
	const String delayParamName{ "delay" };
	const String widthParamName{ "width" };
	const String depthParamName{ "depth" };
	const String numVoicesParamName{ "numVoices" };
	const String lfoFrequencyParamName{ "lfoFrequency" };
	const String lfoWaveformParamName{ "lfoWaveform" };
	const String interpolationParamName{ "interpolation" };
	const String enableStereoParamName{ "enableStereo" };

	const String allowedFileFormat = "*.wch";
	const String presetExtension = ".wch";

	void savePreset()
	{
		const auto state = parameters.valueTreeState.copyState();
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

		parameters.valueTreeState.replaceState(ValueTree::fromXml(*xml));
	}

	void resetPlugin()
	{
		const ValueTree state = parameters.valueTreeState.copyState();
		const std::unique_ptr<XmlElement> tempXml(state.createXml());

		forEachXmlChildElementWithTagName(*tempXml, child, "PARAM")
		{
			const float defaultValue = parameters.valueTreeState.getParameter(child->getStringAttribute("id"))->getDefaultValue();
			const auto range = parameters.valueTreeState.getParameter(child->getStringAttribute("id"))->getNormalisableRange();
			const auto unRangedValue = jmap(defaultValue, range.start, range.end);
			child->setAttribute("value", unRangedValue);
		}

		parameters.valueTreeState.replaceState(ValueTree::fromXml(*tempXml));
	}

private:
    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChorusAudioProcessor)
};
