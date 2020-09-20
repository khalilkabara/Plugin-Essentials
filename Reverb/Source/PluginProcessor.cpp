/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverbAudioProcessor::ReverbAudioProcessor()
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

ReverbAudioProcessor::~ReverbAudioProcessor()
{
}

//==============================================================================
const juce::String ReverbAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool ReverbAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
    return false;
#endif
}

bool ReverbAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
    return false;
#endif
}

bool ReverbAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
	return false;
#endif
}

double ReverbAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int ReverbAudioProcessor::getNumPrograms()
{
	return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int ReverbAudioProcessor::getCurrentProgram()
{
	return 0;
}

void ReverbAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String ReverbAudioProcessor::getProgramName(int index)
{
	return {};
}

void ReverbAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================

void ReverbAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ReverbAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
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
bool ReverbAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ReverbAudioProcessor::createEditor()
{
	return new ReverbAudioProcessorEditor(*this);
}

//=================================PROCESS=============================================

void ReverbAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getTotalNumOutputChannels();

	initDspEffects(spec);
}

void ReverbAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	if (static_cast<float>(*valueTreeState.getRawParameterValue(bypassParamName)) == 1)
	{
		// buffer.clear();
		return;
	}

	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	dsp::AudioBlock<float> block(buffer);
	processDspEffects(dsp::ProcessContextReplacing<float>(block));

	// if(static_cast<float>(*valueTreeState.getRawParameterValue(bypassParamName)) == 0)
	buffer.applyGain(
		Decibels::decibelsToGain(static_cast<float>(*valueTreeState.getRawParameterValue(gainParamName))));

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());
}

//=================================PERSISTENCE=============================================

void ReverbAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
	copyXmlToBinary(*xml, destData);
}

void ReverbAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<XmlElement> loadedParams(getXmlFromBinary(data, sizeInBytes));
	if (loadedParams != nullptr)
		if (loadedParams->hasTagName(valueTreeState.state.getType()))
			valueTreeState.state = ValueTree::fromXml(*loadedParams);
}

//=================================CUSTOM=============================================

void ReverbAudioProcessor::createStateTrees()
{
	using Parameter = AudioProcessorValueTreeState::Parameter;

	NormalisableRange<float> toggleRange{0.0f, 1.0f};
	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(bypassParamName, bypassParamName, bypassParamName,
	                                                                 toggleRange, 0.0f,
	                                                                 [](float val)
	                                                                 {
		                                                                 return String(val, 2);
	                                                                 },
	                                                                 [](String s) { return s.getIntValue(); }));

	NormalisableRange<float> zeroToOneRange{0.0f, 1.0f};

	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(dampingParamName, dampingParamName,
	                                                                 dampingParamName,
	                                                                 zeroToOneRange, 0.5f,
	                                                                 [](float val)
	                                                                 {
		                                                                 return String(val, 2);
	                                                                 },
	                                                                 [](String s) { return s.getIntValue(); }));

	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(roomSizeParamName, roomSizeParamName,
	                                                                 roomSizeParamName,
	                                                                 zeroToOneRange, 0.8f,
	                                                                 [](float val)
	                                                                 {
		                                                                 return String(val, 2);
	                                                                 },
	                                                                 [](String s) { return s.getIntValue(); }));

	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(widthParamName, widthParamName,
	                                                                 widthParamName,
	                                                                 zeroToOneRange, 0.8f,
	                                                                 [](float val)
	                                                                 {
		                                                                 return String(val, 2);
	                                                                 },
	                                                                 [](String s) { return s.getIntValue(); }));

	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(dryLevelParamName, dryLevelParamName,
	                                                                 dryLevelParamName,
	                                                                 zeroToOneRange, 0.5f,
	                                                                 [](float val)
	                                                                 {
		                                                                 return String(val, 2);
	                                                                 },
	                                                                 [](String s) { return s.getIntValue(); }));

	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(wetLevelParamName, wetLevelParamName,
	                                                                 wetLevelParamName,
	                                                                 zeroToOneRange, 0.5,
	                                                                 [](float val)
	                                                                 {
		                                                                 return String(val, 2);
	                                                                 },
	                                                                 [](String s) { return s.getIntValue(); }));

	NormalisableRange<float> gainRange{-24.0f, 24.0f};

	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(gainParamName, gainParamName, gainParamName,
	                                                                 gainRange, 0.0,
	                                                                 [](float val)
	                                                                 {
		                                                                 return String(val, 2);
	                                                                 },
	                                                                 [](String s) { return s.getIntValue(); }));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new ReverbAudioProcessor();
}
