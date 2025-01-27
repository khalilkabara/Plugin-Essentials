/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/


#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

PhaserAudioProcessor::PhaserAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
	AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", AudioChannelSet::stereo(), true)
#endif
	),
#endif
	parameters(*this)
	, paramGain(parameters, "gain", "", -24.0f, 24.0f, 0.0f)
	, paramDepth(parameters, "depth", "", 0.0f, 1.0f, 1.0f)
	, paramFeedback(parameters, "feedback", "", 0.0f, 0.9f, 0.7f)
	, paramNumFilters(parameters, "numFilters", {"2", "4", "6", "8", "10"}, 1,
	                  [this](float value) { return paramNumFilters.items[(int)value].getFloatValue(); })
	, paramMinFrequency(parameters, "frequency", "Hz", 50.0f, 1000.0f, 80.0f)
	, paramSweepWidth(parameters, "sweepWidth", "Hz", 50.0f, 3000.0f, 1000.0f)
	, paramLFOfrequency(parameters, "lfoFrequency", "Hz", 0.0f, 2.0f, 0.05f)
	, paramLFOwaveform(parameters, "lfoWave", waveformItemsUI, waveformSine)
	, paramStereo(parameters, "enableStereo", true)
{
	parameters.valueTreeState.state = ValueTree(Identifier(getName().removeCharacters("- ")));
}

PhaserAudioProcessor::~PhaserAudioProcessor()
{
}

//==============================================================================

void PhaserAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	const double smoothTime = 1e-3;
	paramGain.reset(sampleRate, smoothTime);
	paramDepth.reset(sampleRate, smoothTime);
	paramFeedback.reset(sampleRate, smoothTime);
	paramNumFilters.reset(sampleRate, smoothTime);
	paramMinFrequency.reset(sampleRate, smoothTime);
	paramSweepWidth.reset(sampleRate, smoothTime);
	paramLFOfrequency.reset(sampleRate, smoothTime);
	paramLFOwaveform.reset(sampleRate, smoothTime);
	paramStereo.reset(sampleRate, smoothTime);

	//======================================

	numFiltersPerChannel = paramNumFilters.callback(paramNumFilters.items.size() - 1);

	filters.clear();
	for (int i = 0; i < getTotalNumInputChannels() * numFiltersPerChannel; ++i)
	{
		Filter* filter;
		filters.add(filter = new Filter());
	}

	filteredOutputs.clear();
	for (int i = 0; i < getTotalNumInputChannels(); ++i)
		filteredOutputs.add(0.0f);

	sampleCountToUpdateFilters = 0;
	updateFiltersInterval = 32;

	lfoPhase = 0.0f;
	inverseSampleRate = 1.0f / (float)sampleRate;
	twoPi = 2.0f * M_PI;
}

void PhaserAudioProcessor::releaseResources()
{
}

void PhaserAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	ScopedNoDenormals noDenormals;

	const int numInputChannels = getTotalNumInputChannels();
	const int numOutputChannels = getTotalNumOutputChannels();
	const int numSamples = buffer.getNumSamples();

	//======================================

	float phase;
	float phaseMain;
	unsigned int sampleCount;

	for (int channel = 0; channel < numInputChannels; ++channel)
	{
		float* channelData = buffer.getWritePointer(channel);
		sampleCount = sampleCountToUpdateFilters;
		phase = lfoPhase;
		if ((bool)paramStereo.getTargetValue() && channel != 0)
			phase = fmodf(phase + 0.25f, 1.0f);

		for (int sample = 0; sample < numSamples; ++sample)
		{
			float in = channelData[sample];

			float centreFrequency = lfo(phase, (int)paramLFOwaveform.getTargetValue());
			centreFrequency *= paramSweepWidth.getNextValue();
			centreFrequency += paramMinFrequency.getNextValue();

			phase += paramLFOfrequency.getNextValue() * inverseSampleRate;
			if (phase >= 1.0f)
				phase -= 1.0f;

			if (sampleCount++ % updateFiltersInterval == 0)
				updateFilters(centreFrequency);

			float filtered = in + paramFeedback.getNextValue() * filteredOutputs[channel];
			for (int i = 0; i < paramNumFilters.getTargetValue(); ++i)
				filtered = filters[channel * paramNumFilters.getTargetValue() + i]->processSingleSampleRaw(filtered);

			filteredOutputs.set(channel, filtered);
			float out = in + paramDepth.getNextValue() * (filtered - in) * 0.5f;
			channelData[sample] = out;
		}

		if (channel == 0)
			phaseMain = phase;
	}
	// buffer.applyGain(2);
	buffer.applyGain(Decibels::decibelsToGain(paramGain.getNextValue()));

	sampleCountToUpdateFilters = sampleCount;
	lfoPhase = phaseMain;

	for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
		buffer.clear(channel, 0, numSamples);
}

//==============================================================================

void PhaserAudioProcessor::updateFilters(double centreFrequency)
{
	double discreteFrequency = twoPi * centreFrequency * inverseSampleRate;

	for (int i = 0; i < filters.size(); ++i)
		filters[i]->updateCoefficients(discreteFrequency);
}

//==============================================================================

float PhaserAudioProcessor::lfo(float phase, int waveform)
{
	float out = 0.0f;

	switch (waveform)
	{
	case waveformSine:
		{
			out = 0.5f + 0.5f * sinf(twoPi * phase);
			break;
		}
	case waveformTriangle:
		{
			if (phase < 0.25f)
				out = 0.5f + 2.0f * phase;
			else if (phase < 0.75f)
				out = 1.0f - 2.0f * (phase - 0.25f);
			else
				out = 2.0f * (phase - 0.75f);
			break;
		}
	case waveformSquare:
		{
			if (phase < 0.5f)
				out = 1.0f;
			else
				out = 0.0f;
			break;
		}
	case waveformSawtooth:
		{
			if (phase < 0.5f)
				out = 0.5f + phase;
			else
				out = phase - 0.5f;
			break;
		}
	}

	return out;
}

//==============================================================================

void PhaserAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	auto state = parameters.valueTreeState.copyState();
	std::unique_ptr<XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void PhaserAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(parameters.valueTreeState.state.getType()))
			parameters.valueTreeState.replaceState(ValueTree::fromXml(*xmlState));
}

//==============================================================================

AudioProcessorEditor* PhaserAudioProcessor::createEditor()
{
	return new PhaserAudioProcessorEditor(*this);
}

bool PhaserAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

//==============================================================================

#ifndef JucePlugin_PreferredChannelConfigurations
bool PhaserAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

const String PhaserAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool PhaserAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool PhaserAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool PhaserAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double PhaserAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

//==============================================================================

int PhaserAudioProcessor::getNumPrograms()
{
	return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int PhaserAudioProcessor::getCurrentProgram()
{
	return 0;
}

void PhaserAudioProcessor::setCurrentProgram(int index)
{
}

const String PhaserAudioProcessor::getProgramName(int index)
{
	return {};
}

void PhaserAudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new PhaserAudioProcessor();
}

//==============================================================================
