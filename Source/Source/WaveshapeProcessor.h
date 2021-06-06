#pragma once
#include "Parameters.h"

struct WaveshapeProcessor : AudioProcessorValueTreeState::Listener {
	WaveshapeProcessor(AudioProcessorValueTreeState& apvts) : apvts_(apvts) {
		apvts.addParameterListener(WAVECHOICE_ID, this);
		apvts.addParameterListener(TANHMULT_ID, this);
		apvts.addParameterListener(SINFREQ_ID, this);
		apvts.addParameterListener(SINAMOUNT_ID, this);
	}

	float Process(float sample) {
		float drySample = sample;

		//Tanh function applied to the signal
		sample = std::tanh(sample * tanhMult_);

		//if (waveChoice_ == -1) {
		//	//Triangle (not really a pure triangle)
		//	sample += sinAmount_ * std::asin(std::sin(drySample * (sinFreq_ - 4.f)));
		//	sample += sinAmount_ * std::sin(drySample * sinFreq_ * 2.f);
		//}
		//else if (waveChoice_ == 0) {
			//Sinewave
			//sample += sinAmount_ * std::sin(drySample * sinFreq_);
		//}

		return juce::jlimit(-1.f, 1.f, sample);
	}

private:
	AudioProcessorValueTreeState& apvts_;
	float waveChoice_, tanhMult_, sinFreq_, sinAmount_;

	void parameterChanged(const String& id, float val) override {
		if (id == WAVECHOICE_ID) {
			waveChoice_ = val;
		}
		else if (id == TANHMULT_ID) {
			tanhMult_ = val;
		}
		else if (id == SINFREQ_ID) {
			sinFreq_ = val;
		}
		else if (id == SINAMOUNT_ID) {
			sinAmount_ = val * 0.005f;
		}
	}
};