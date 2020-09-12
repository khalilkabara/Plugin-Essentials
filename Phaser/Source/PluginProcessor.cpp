/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/


#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginParameter.h"

//==============================================================================

PhaserAudioProcessor::PhaserAudioProcessor():
#ifndef JucePlugin_PreferredChannelConfigurations
	AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", AudioChannelSet::stereo(), true)
#endif
	), valueTreeState(*this, &undoManager)
#endif
{
	createStateTrees();
	valueTreeState.state = ValueTree(SAVED_PARAMS_NAME);
}

PhaserAudioProcessor::~PhaserAudioProcessor()
{
}

void PhaserAudioProcessor::releaseResources()
{
}

AudioProcessorEditor* PhaserAudioProcessor::createEditor()
{
	return new PhaserAudioProcessorEditor(*this);
}

bool PhaserAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PhaserAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
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

//=================================PROCESS=============================================

void PhaserAudioProcessor::updateFilters(double centreFrequency) const
{
	double discreteFrequency = twoPi * centreFrequency * inverseSampleRate;

	for (int i = 0; i < filters.size(); ++i)
		filters[i]->updateCoefficients(discreteFrequency);
}

float PhaserAudioProcessor::lfo(float phase, int waveform) const
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

void PhaserAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	updatePluginParams();

	// const double smoothTime = 1e-3;
	// paramDepth.reset(sampleRate, smoothTime);
	// paramFeedback.reset(sampleRate, smoothTime);
	// paramNumFilters.reset(sampleRate, smoothTime);
	// paramMinFrequency.reset(sampleRate, smoothTime);
	// paramSweepWidth.reset(sampleRate, smoothTime);
	// paramLFOfrequency.reset(sampleRate, smoothTime);
	// paramLFOwaveform.reset(sampleRate, smoothTime);
	// paramStereo.reset(sampleRate, smoothTime);
	//
	// //======================================
	//
	// numFiltersPerChannel = paramNumFilters.callback(paramNumFilters.items.size() - 1);

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
	updatePluginParams();

	for (int channel = 0; channel < numInputChannels; ++channel)
	{
		float* channelData = buffer.getWritePointer(channel);
		sampleCount = sampleCountToUpdateFilters;
		phase = lfoPhase;
		if (paramStereo == 1 && channel != 0)
			phase = fmodf(phase + 0.25f, 1.0f);

		for (int sample = 0; sample < numSamples; ++sample)
		{
			float in = channelData[sample];

			float centreFrequency = lfo(phase, static_cast<int>(paramLFOwaveform));
			centreFrequency *= paramSweepWidth;
			centreFrequency += paramMinFrequency;

			phase += paramLFOfrequency * inverseSampleRate;
			if (phase >= 1.0f)
				phase -= 1.0f;

			if (sampleCount++ % updateFiltersInterval == 0)
				updateFilters(centreFrequency);

			float filtered = in + paramFeedback * filteredOutputs[channel];
			for (int i = 0; i < paramNumFilters; ++i)
				filtered = filters[channel * paramNumFilters + i]->processSingleSampleRaw(filtered);

			filteredOutputs.set(channel, filtered);
			float out = in + paramDepth * (filtered - in) * 0.5f;
			channelData[sample] = out;
		}

		if (channel == 0)
			phaseMain = phase;
	}

	sampleCountToUpdateFilters = sampleCount;
	lfoPhase = phaseMain;

	for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
		buffer.clear(channel, 0, numSamples);
}

//=================================PERSISTENCE=============================================

void PhaserAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
	copyXmlToBinary(*xml, destData);
}

void PhaserAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<XmlElement> loadedParams(getXmlFromBinary(data, sizeInBytes));
	if (loadedParams != nullptr)
		if (loadedParams->hasTagName(valueTreeState.state.getType()))
			valueTreeState.state = ValueTree::fromXml(*loadedParams);
}

//=================================CUSTOM=============================================

void PhaserAudioProcessor::updatePluginParams()
{
	paramDepth = static_cast<float>(*valueTreeState.getRawParameterValue(depthParamName));
	paramFeedback = static_cast<float>(*valueTreeState.getRawParameterValue(feedbackParamName));
	paramNumFilters = static_cast<float>(*valueTreeState.getRawParameterValue(numFiltersParamName));
	paramMinFrequency = static_cast<float>(*valueTreeState.getRawParameterValue(minFrequencyParamName));
	paramSweepWidth = static_cast<float>(*valueTreeState.getRawParameterValue(sweepWidthParamName));
	paramLFOfrequency = static_cast<float>(*valueTreeState.getRawParameterValue(lfoFrequencyParamName));
	paramLFOwaveform = static_cast<float>(*valueTreeState.getRawParameterValue(lfoWaveformParamName));
	paramStereo = static_cast<float>(*valueTreeState.getRawParameterValue(useStereoParamName));
}

void PhaserAudioProcessor::createStateTrees()
{
	using Parameter = AudioProcessorValueTreeState::Parameter;

	NormalisableRange<float> zeroToOneParam(zeroToOneMinValue, zeroToOneMaxValue);
	NormalisableRange<float> minFrequencyParam(minFrequencyMinValue, minFrequencyMaxValue);
	NormalisableRange<float> sweepWidthParam(sweepWidthMinValue, sweepWidthMaxValue);
	NormalisableRange<float> lfoFrequencyParam(lfoFrequencyMinValue, lfoFrequencyMaxValue);
	NormalisableRange<float> toggleParam(toggleMinValue, toggleMaxValue);
	NormalisableRange<float> numFiltersParam(0, NUM_FILTERS.size() - 1);
	NormalisableRange<float> lfoWaveformParam(0, LFO_WAVEFORMS.size() - 1);

	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(numFiltersParamName, numFiltersParamName,
	                                                                 numFiltersParamName, numFiltersParam,
	                                                                 0, nullptr,
	                                                                 nullptr));

	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(lfoWaveformParamName, lfoWaveformParamName,
	                                                                 lfoWaveformParamName, lfoWaveformParam,
	                                                                 0, nullptr,
	                                                                 nullptr));

	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(useStereoParamName, useStereoParamName,
	                                                                 useStereoParamName, toggleParam,
	                                                                 toggleMinValue, nullptr,
	                                                                 nullptr));

	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(depthParamName, depthParamName,
	                                                                 depthParamName, zeroToOneParam,
	                                                                 zeroToOneDefaultValue, nullptr,
	                                                                 nullptr));

	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(feedbackParamName, feedbackParamName,
	                                                                 feedbackParamName, zeroToOneParam,
	                                                                 zeroToOneDefaultValue, nullptr,
	                                                                 nullptr));

	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(dryWetParamName, dryWetParamName,
	                                                                 dryWetParamName, zeroToOneParam,
	                                                                 zeroToOneDefaultValue, nullptr,
	                                                                 nullptr));

	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(minFrequencyParamName, minFrequencyParamName,
	                                                                 minFrequencyParamName, minFrequencyParam,
	                                                                 minFrequencyMinValue, nullptr,
	                                                                 nullptr));

	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(sweepWidthParamName, sweepWidthParamName,
	                                                                 sweepWidthParamName, sweepWidthParam,
	                                                                 sweepWidthDefaultValue, nullptr,
	                                                                 nullptr));

	valueTreeState.createAndAddParameter(std::make_unique<Parameter>(lfoFrequencyParamName, lfoFrequencyParamName,
	                                                                 lfoFrequencyParamName, lfoFrequencyParam,
	                                                                 lfoFrequencyDefaultValue, nullptr,
	                                                                 nullptr));
}

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new PhaserAudioProcessor();
}

//==============================================================================
