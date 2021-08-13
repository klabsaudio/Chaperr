#pragma once
#include "Parameters.h"

struct LowPassFilter : AudioProcessorValueTreeState::Listener, juce::OwnedArray<juce::IIRFilter> {
	LowPassFilter(juce::AudioProcessorValueTreeState& apvts, int numChannels) : apvts_(apvts) {
		apvts.addParameterListener(LPCUTOFF_ID, this);
		apvts.addParameterListener(LPRESO_ID, this);

		cutoff_.setTargetValue(*apvts_.getRawParameterValue(LPCUTOFF_ID));
		reso_.setTargetValue(*apvts_.getRawParameterValue(LPRESO_ID));

		for (int i = 0; i < numChannels; i++) {
			this->add(new juce::IIRFilter());
			(*this)[i]->reset();
			(*this)[i]->setCoefficients(IIRCoefficients::makeLowPass(sampleRate_, cutoff_.getTargetValue(), reso_.getTargetValue()));
		}
	}

	void reset(float sampleRate) {
		sampleRate_ = sampleRate;
		cutoff_.reset(sampleRate, 0.1f);
		reso_.reset(sampleRate, 0.1f);
	}

	float Process(int channel, float sample) {
		if (cutoff_.isSmoothing() || reso_.isSmoothing()) {
			for (auto i : (*this)) {
				i->setCoefficients(IIRCoefficients::makeLowPass(sampleRate_, cutoff_.getNextValue(), reso_.getNextValue()));
			}
		}

		return (*this)[channel]->processSingleSampleRaw(sample);
	}

private:
	juce::AudioProcessorValueTreeState& apvts_;
	float sampleRate_ = 44100.f;
	juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> cutoff_, reso_;

	void parameterChanged(const String& id, float val) override {
		if (id == LPCUTOFF_ID) {
			cutoff_.setTargetValue(val);
		}
		else if (id == LPRESO_ID) {
			reso_.setTargetValue(val);
		}
	}
};