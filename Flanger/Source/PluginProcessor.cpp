/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/


#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

FlangerAudioProcessor::FlangerAudioProcessor():
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor (BusesProperties()
                    #if ! JucePlugin_IsMidiEffect
                     #if ! JucePlugin_IsSynth
                      .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     #endif
                      .withOutput ("Output", AudioChannelSet::stereo(), true)
                    #endif
                   ),
#endif
    parameters (*this)
	, paramGain(parameters, "gain", "db", -24.0f, 24.0f, 0.0f)
    , paramDelay (parameters, "delay", "ms", 1.0f, 20.0f, 2.5f, [](float value){ return value * 0.001f; })
    , paramWidth (parameters, "width", "ms", 1.0f, 20.0f, 10.0f, [](float value){ return value * 0.001f; })
    , paramDepth (parameters, "depth", "", 0.0f, 1.0f, 1.0f)
    , paramFeedback (parameters, "feedback", "", 0.0f, 0.5f, 0.0f)
    , paramInverted (parameters, "enableInverted", false, [](float value){ return value * (-2.0f) + 1.0f; })
    , paramFrequency (parameters, "lfoFrequency", "Hz", 0.05f, 2.0f, 0.2f)
    , paramWaveform (parameters, "lfoWave", waveformItemsUI, waveformSine)
    , paramInterpolation (parameters, "interpolation", interpolationItemsUI, interpolationLinear)
    , paramStereo (parameters, "enableStereo")
{
    parameters.valueTreeState.state = ValueTree (Identifier (getName().removeCharacters ("- ")));
}

FlangerAudioProcessor::~FlangerAudioProcessor()
{
}

//==============================================================================

void FlangerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const double smoothTime = 1e-3;
    paramDelay.reset (sampleRate, smoothTime);
    paramWidth.reset (sampleRate, smoothTime);
    paramDepth.reset (sampleRate, smoothTime);
    paramFeedback.reset (sampleRate, smoothTime);
    paramInverted.reset (sampleRate, smoothTime);
    paramFrequency.reset (sampleRate, smoothTime);
    paramWaveform.reset (sampleRate, smoothTime);
    paramInterpolation.reset (sampleRate, smoothTime);
    paramStereo.reset (sampleRate, smoothTime);

    //======================================

    float maxDelayTime = paramDelay.maxValue + paramWidth.maxValue;
    delayBufferSamples = (int)(maxDelayTime * (float)sampleRate) + 1;
    if (delayBufferSamples < 1)
        delayBufferSamples = 1;

    delayBufferChannels = getTotalNumInputChannels();
    delayBuffer.setSize (delayBufferChannels, delayBufferSamples);
    delayBuffer.clear();

    delayWritePosition = 0;
    lfoPhase = 0.0f;
    inverseSampleRate = 1.0f / (float)sampleRate;
    twoPi = 2.0f * M_PI;
}

void FlangerAudioProcessor::releaseResources()
{
}

void FlangerAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    const int numInputChannels = getTotalNumInputChannels();
    const int numOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    //======================================

    float currentDelay = paramDelay.getNextValue();
    float currentWidth = paramWidth.getNextValue();
    float currentDepth = paramDepth.getNextValue();
    float currentFeedback = paramFeedback.getNextValue();
    float currentInverted = paramInverted.getNextValue();
    float currentFrequency = paramFrequency.getNextValue();

    int localWritePosition;
    float phase;
    float phaseMain;

    for (int channel = 0; channel < numInputChannels; ++channel) {
        float* channelData = buffer.getWritePointer (channel);
        float* delayData = delayBuffer.getWritePointer (channel);
        localWritePosition = delayWritePosition;
        phase = lfoPhase;
        if ((bool)paramStereo.getTargetValue() && channel != 0)
            phase = fmodf (phase + 0.25f, 1.0f);

        for (int sample = 0; sample < numSamples; ++sample) {
            const float in = channelData[sample];
            float out = 0.0f;

            float localDelayTime =
                (currentDelay + currentWidth * lfo (phase, (int)paramWaveform.getTargetValue())) * (float)getSampleRate();

            float readPosition =
                fmodf ((float)localWritePosition - localDelayTime + (float)delayBufferSamples, delayBufferSamples);
            int localReadPosition = floorf (readPosition);

            switch ((int)paramInterpolation.getTargetValue()) {
                case interpolationNearestNeighbour: {
                    float closestSample = delayData[localReadPosition % delayBufferSamples];
                    out = closestSample;
                    break;
                }
                case interpolationLinear: {
                    float fraction = readPosition - (float)localReadPosition;
                    float delayed0 = delayData[(localReadPosition + 0)];
                    float delayed1 = delayData[(localReadPosition + 1) % delayBufferSamples];
                    out = delayed0 + fraction * (delayed1 - delayed0);
                    break;
                }
                case interpolationCubic: {
                    float fraction = readPosition - (float)localReadPosition;
                    float fractionSqrt = fraction * fraction;
                    float fractionCube = fractionSqrt * fraction;

                    float sample0 = delayData[(localReadPosition - 1 + delayBufferSamples) % delayBufferSamples];
                    float sample1 = delayData[(localReadPosition + 0)];
                    float sample2 = delayData[(localReadPosition + 1) % delayBufferSamples];
                    float sample3 = delayData[(localReadPosition + 2) % delayBufferSamples];

                    float a0 = - 0.5f * sample0 + 1.5f * sample1 - 1.5f * sample2 + 0.5f * sample3;
                    float a1 = sample0 - 2.5f * sample1 + 2.0f * sample2 - 0.5f * sample3;
                    float a2 = - 0.5f * sample0 + 0.5f * sample2;
                    float a3 = sample1;
                    out = a0 * fractionCube + a1 * fractionSqrt + a2 * fraction + a3;
                    break;
                }
            }

            channelData[sample] = in + out * currentDepth * currentInverted;
            delayData[localWritePosition] = in + out * currentFeedback;

            if (++localWritePosition >= delayBufferSamples)
                localWritePosition -= delayBufferSamples;

            phase += currentFrequency * inverseSampleRate;
            if (phase >= 1.0f)
                phase -= 1.0f;
        }

        if (channel == 0)
            phaseMain = phase;
    }

    delayWritePosition = localWritePosition;
    lfoPhase = phaseMain;

    //======================================

	buffer.applyGain(Decibels::decibelsToGain(paramGain.getNextValue()));
	
    for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
        buffer.clear (channel, 0, numSamples);
}

//==============================================================================

float FlangerAudioProcessor::lfo (float phase, int waveform)
{
    float out = 0.0f;

    switch (waveform) {
        case waveformSine: {
            out = 0.5f + 0.5f * sinf (twoPi * phase);
            break;
        }
        case waveformTriangle: {
            if (phase < 0.25f)
                out = 0.5f + 2.0f * phase;
            else if (phase < 0.75f)
                out = 1.0f - 2.0f * (phase - 0.25f);
            else
                out = 2.0f * (phase - 0.75f);
            break;
        }
        case waveformSawtooth: {
            if (phase < 0.5f)
                out = 0.5f + phase;
            else
                out = phase - 0.5f;
            break;
        }
        case waveformInverseSawtooth: {
            if (phase < 0.5f)
                out = 0.5f - phase;
            else
                out = 1.5f - phase;
            break;
        }
    }

    return out;
}

//==============================================================================

void FlangerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto state = parameters.valueTreeState.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void FlangerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.valueTreeState.state.getType()))
            parameters.valueTreeState.replaceState (ValueTree::fromXml (*xmlState));
}

//==============================================================================

AudioProcessorEditor* FlangerAudioProcessor::createEditor()
{
    return new FlangerAudioProcessorEditor (*this);
}

bool FlangerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

//==============================================================================

#ifndef JucePlugin_PreferredChannelConfigurations
bool FlangerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

//==============================================================================

const String FlangerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FlangerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FlangerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FlangerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FlangerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================

int FlangerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FlangerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FlangerAudioProcessor::setCurrentProgram (int index)
{
}

const String FlangerAudioProcessor::getProgramName (int index)
{
    return {};
}

void FlangerAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FlangerAudioProcessor();
}

//==============================================================================
