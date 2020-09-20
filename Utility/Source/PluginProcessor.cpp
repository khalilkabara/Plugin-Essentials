/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
UtilityAudioProcessor::UtilityAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		  .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		  .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	  ), valueTreeState(*this, &undoManager)
#endif
{
	createStateTrees();
	valueTreeState.state = ValueTree(SAVED_PARAMS_NAME);
}

UtilityAudioProcessor::~UtilityAudioProcessor()
{
}

//==============================================================================
const juce::String UtilityAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool UtilityAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
    return false;
#endif
}

bool UtilityAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
    return false;
#endif
}

bool UtilityAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
	return false;
#endif
}

double UtilityAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int UtilityAudioProcessor::getNumPrograms()
{
	return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int UtilityAudioProcessor::getCurrentProgram()
{
	return 0;
}

void UtilityAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String UtilityAudioProcessor::getProgramName(int index)
{
	return {};
}

void UtilityAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================

void UtilityAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UtilityAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

//==============================================================================
bool UtilityAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* UtilityAudioProcessor::createEditor()
{
	return new UtilityAudioProcessorEditor(*this);
}

//=================================PROCESS=============================================

void UtilityAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getTotalNumOutputChannels();

	initDspEffects(spec);
}

void UtilityAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	if (static_cast<float>(*valueTreeState.getRawParameterValue(muteParamName)) == 1)
	{
		buffer.clear();
		return;
	}
	
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	dsp::AudioBlock<float> block(buffer);
	processDspEffects(dsp::ProcessContextReplacing<float>(block));

	buffer.applyGain(Decibels::decibelsToGain(static_cast<float>(*valueTreeState.getRawParameterValue(gainParamName))));

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

}

//=================================PERSISTENCE=============================================

void UtilityAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
	copyXmlToBinary(*xml, destData);
}

void UtilityAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<XmlElement> loadedParams(getXmlFromBinary(data, sizeInBytes));
	if (loadedParams != nullptr)
		if (loadedParams->hasTagName(valueTreeState.state.getType()))
			valueTreeState.state = ValueTree::fromXml(*loadedParams);
}

//=================================CUSTOM=============================================

void UtilityAudioProcessor::createStateTrees()
{
	using Parameter = AudioProcessorValueTreeState::Parameter;

	NormalisableRange<float> gainRange{-48.0f, 48.0f};
	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(gainParamName, gainParamName, gainParamName,
	                                                                 gainRange, 0.0,
	                                                                 [](float val)
	                                                                 {
		                                                                 return String(val, 2);
	                                                                 },
	                                                                 [](String s) { return s.getIntValue(); }));

	NormalisableRange<float> panRange{-1.0f, 1.0f};
	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(panParamName, panParamName, panParamName,
	                                                                 panRange, 0.0f,
	                                                                 [](float val)
	                                                                 {
		                                                                 return String(val, 2);
	                                                                 },
	                                                                 [](String s) { return s.getIntValue(); }));

	NormalisableRange<float> toggleRange{0.0f, 1.0f};
	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(muteParamName, muteParamName, muteParamName,
	                                                                 toggleRange, 0.0f,
	                                                                 [](float val)
	                                                                 {
		                                                                 return String(val, 2);
	                                                                 },
	                                                                 [](String s) { return s.getIntValue(); }));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new UtilityAudioProcessor();
}
