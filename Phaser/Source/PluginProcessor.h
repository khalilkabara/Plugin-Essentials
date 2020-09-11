/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/


#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginParameter.h"

//==============================================================================

class PhaserAudioProcessor : public AudioProcessor
{
public:
	//==============================================================================

	PhaserAudioProcessor();
	~PhaserAudioProcessor();

	//==============================================================================

	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;
	void processBlock(AudioSampleBuffer&, MidiBuffer&) override;

	//==============================================================================

	void getStateInformation(MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	//==============================================================================

	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	//==============================================================================

	const String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================

	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const String getProgramName(int index) override;
	void changeProgramName(int index, const String& newName) override;

	//==============================================================================

	StringArray waveformItemsUI = {
		"Sine",
		"Triangle",
		"Square",
		"Sawtooth"
	};

	enum waveformIndex
	{
		waveformSine = 0,
		waveformTriangle,
		waveformSquare,
		waveformSawtooth,
	};

	//======================================

	class Filter : public IIRFilter
	{
	public:
		void updateCoefficients(const double discreteFrequency) noexcept
		{
			jassert(discreteFrequency > 0);

			double wc = jmin(discreteFrequency, M_PI * 0.99);
			double tan_half_wc = tan(wc / 2.0);

			coefficients = IIRCoefficients(/* b0 */ tan_half_wc - 1.0,
			                                        /* b1 */ tan_half_wc + 1.0,
			                                        /* b2 */ 0.0,
			                                        /* a0 */ tan_half_wc + 1.0,
			                                        /* a1 */ tan_half_wc - 1.0,
			                                        /* a2 */ 0.0);

			setCoefficients(coefficients);
		}
	};

	OwnedArray<Filter> filters;
	Array<float> filteredOutputs;
	void updateFilters(double centreFrequency) const;
	int numFiltersPerChannel;
	unsigned int sampleCountToUpdateFilters;
	unsigned int updateFiltersInterval;

	float lfoPhase;
	float inverseSampleRate;
	float twoPi;

	float lfo(float phase, int waveform) const;

	//======================================

	// PluginParametersManager parameters;
	
	float paramDepth;
	float paramFeedback;
	float paramNumFilters;
	float paramMinFrequency;
	float paramSweepWidth;
	float paramLFOfrequency;
	float paramLFOwaveform;
	float paramStereo;

	//******************************************************************************

	AudioProcessorValueTreeState valueTreeState;
	UndoManager undoManager;
	const String SAVED_PARAMS_NAME{"savedParams"};

	const String numFiltersParamName{"numFilters"};
	const String lfoWaveformParamName{"lfoWaveform"};
	const String useStereoParamName{"useStereo"};
	const String depthParamName{"depth"};
	const String feedbackParamName{"feedback"};
	const String minFrequencyParamName{"minFrequency"};
	const String sweepWidthParamName{"sweepWidth"};
	const String lfoFrequencyParamName{"lfoFrequency"};

	// feedback 0.0~0.9
	// min freq 0.0~1000, sweep width 0 - 3000, lfo freq 0 - 2hz

	const StringArray NUM_FILTERS{"2", "4", "6", "8", "10"};
	const StringArray LFO_WAVEFORMS{"Sin", "Saw", "Triangle", "Square"};
	const float zeroToOneMinValue = 0.0;
	const float zeroToOneMaxValue = 1.0;
	const float zeroToOneDefaultValue = 0.8;
	const float zeroToOneMidpointValue = 0.3;
	const float zeroToOneStepValue = 0.01;

	const float minFrequencyMinValue = 0.0;
	const float minFrequencyMaxValue = 1000.0;
	const float minFrequencyDefaultValue = 250.0;
	const float minFrequencyMidpointValue = 250.0;
	const float minFrequencyStepValue = 1.0;

	const float sweepWidthMinValue = 0.0;
	const float sweepWidthMaxValue = 3000.0;
	const float sweepWidthDefaultValue = 500.0;
	const float sweepWidthMidpointValue = 500.0;
	const float sweepWidthStepValue = 1.0;

	const float lfoFrequencyMinValue = 0.0;
	const float lfoFrequencyMaxValue = 2.0;
	const float lfoFrequencyDefaultValue = 0.5;
	const float lfoFrequencyMidpointValue = 0.5;
	const float lfoFrequencyStepValue = 0.01;

	const float toggleMinValue = 0.0;
	const float toggleMaxValue = 1.0;
	const float toggleDefaultValue = 1.0;
	const float toggleStepValue = 1.0;

	//******************************************************************************


private:
	//==============================================================================

	void updatePluginParams();
	void createStateTrees();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhaserAudioProcessor)
};
