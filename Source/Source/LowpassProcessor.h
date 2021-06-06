#pragma once
#include "Parameters.h"

struct LowPassFilter : AudioProcessorValueTreeState::Listener, juce::OwnedArray<IIRFilter> {
	LowPassFilter(juce::AudioProcessorValueTreeState& apvts, int numChannels) : apvts_(apvts) {
		apvts.addParameterListener(LPCUTOFF_ID, this);
		apvts.addParameterListener(LPRESO_ID, this);
		apvts.addParameterListener(LPBYPASS_ID, this);

		cutoff_ = *apvts_.getRawParameterValue(LPCUTOFF_ID);
		reso_ = *apvts_.getRawParameterValue(LPRESO_ID);

		for (int i = 0; i < numChannels; i++) {
			this->add(new IIRFilter());
			(*this)[i]->reset();
		}
	}

	void setSampleRate(float sampleRate) { sampleRate_ = sampleRate; }

private:
	juce::AudioProcessorValueTreeState& apvts_;
	float cutoff_, reso_, sampleRate_ = 44100.f;
	bool bypass_;

	void parameterChanged(const String& id, float val) override {
		if (id == LPCUTOFF_ID) {
			cutoff_ = val;
		}
		else if (id == LPRESO_ID) {
			reso_ = val;
		}
		else if (id == LPBYPASS_ID) {
			bypass_ = bool(val);
		}

		for (int i = 0; i < this->size(); i++) {
			(*this)[i]->setCoefficients(IIRCoefficients::makeLowPass(sampleRate_, cutoff_, reso_));
		}
	}
};