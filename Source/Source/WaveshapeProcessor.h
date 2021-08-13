#pragma once
#include "Parameters.h"

struct WaveshapeProcessor : AudioProcessorValueTreeState::Listener {
	WaveshapeProcessor(AudioProcessorValueTreeState& apvts) : apvts_(apvts) {
		apvts.addParameterListener(WAVECHOICE_ID, this);
		apvts.addParameterListener(TANHMULT_ID, this);
		apvts.addParameterListener(SINFREQ_ID, this);
		apvts.addParameterListener(SINAMOUNT_ID, this);

		waveChoice_ = *apvts.getRawParameterValue(WAVECHOICE_ID);
		tanhMult_.setValue(*apvts.getRawParameterValue(TANHMULT_ID));
		sinFreq_.setValue(*apvts.getRawParameterValue(SINFREQ_ID));
		sinAmount_.setValue(*apvts.getRawParameterValue(SINAMOUNT_ID));

		//samplerate hardcoded but only for interpolation
		tanhMult_.reset(48000, 0.1f);
		sinFreq_.reset(48000, 0.1f);
		sinAmount_.reset(48000, 0.1f);
	}

	float Process(float sample) {
		float drySample = sample;

		//Tanh function applied to the signal
		sample = std::tanh(sample * tanhMult_.getNextValue());

		if (waveChoice_ == -1) {
			//Triangle (not really a pure triangle)
			sample += sinAmount_.getNextValue() * 0.005f * std::asin(std::sin(drySample * (sinFreq_.getNextValue() - 4.f)));
			sample += sinAmount_.getNextValue() * 0.005f * std::sin(drySample * sinFreq_.getNextValue() * 2.f);
		}
		else if (waveChoice_ == 0) {
			//Sinewave
			sample += sinAmount_.getNextValue() * 0.005f * std::sin(drySample * sinFreq_.getNextValue());
		}

		return juce::jlimit(-1.f, 1.f, sample);
	}

private:
	AudioProcessorValueTreeState& apvts_;
	SmoothedValue<float, juce::ValueSmoothingTypes::Linear> tanhMult_, sinFreq_, sinAmount_;
	float waveChoice_;

	void parameterChanged(const String& id, float val) override {
		if (id == WAVECHOICE_ID) {
			waveChoice_ = val;
		}
		else if (id == TANHMULT_ID) {
			tanhMult_.setTargetValue(val);
		}
		else if (id == SINFREQ_ID) {
			sinFreq_.setTargetValue(val);
		}
		else if (id == SINAMOUNT_ID) {
			sinAmount_.setTargetValue(val);
		}
	}
};