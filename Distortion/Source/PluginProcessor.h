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

class DistortionAudioProcessor : public AudioProcessor
{
public:
    //==============================================================================

    DistortionAudioProcessor();
    ~DistortionAudioProcessor();

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

    StringArray distortionTypeItemsUI = {
        "Hard clipping",
        "Soft clipping",
        "Exponential",
        "Full-wave rectifier",
        "Half-wave rectifier"
    };

    enum distortionTypeIndex {
        distortionTypeHardClipping = 0,
        distortionTypeSoftClipping,
        distortionTypeExponential,
        distortionTypeFullWaveRectifier,
        distortionTypeHalfWaveRectifier,
    };

    //======================================

    class Filter : public IIRFilter
    {
    public:
        void updateCoefficients (const double discreteFrequency,
                                 const double gain) noexcept
        {
            jassert (discreteFrequency > 0);

            double tan_half_wc = tan (discreteFrequency / 2.0);
            double sqrt_gain = sqrt (gain);

            coefficients = IIRCoefficients (/* b0 */ sqrt_gain * tan_half_wc + gain,
                                            /* b1 */ sqrt_gain * tan_half_wc - gain,
                                            /* b2 */ 0.0,
                                            /* a0 */ sqrt_gain * tan_half_wc + 1.0,
                                            /* a1 */ sqrt_gain * tan_half_wc - 1.0,
                                            /* a2 */ 0.0);

            setCoefficients (coefficients);
        }
    };

    OwnedArray<Filter> filters;
    void updateFilters();

    //======================================

    PluginParametersManager parameters;

    PluginParameterComboBox paramDistortionType;
    PluginParameterLinSlider paramInputGain;
    PluginParameterLinSlider paramOutputGain;
    PluginParameterLinSlider paramTone;

	const String distortionTypeParamName{ "distortionType" };
	const String inputGainParamName{ "inGain" };
	const String toneParamName{ "tone" };
	const String outputGainParamName{ "outGain" };

	const String allowedFileFormat = "*.wdt";
	const String presetExtension = ".wdt";

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionAudioProcessor)
};
