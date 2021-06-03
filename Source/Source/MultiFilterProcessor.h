#pragma once
#include "Parameters.h"

struct MultiFilter : AudioProcessorValueTreeState::Listener {
	MultiFilter(juce::AudioProcessorValueTreeState& apvts) : apvts_(apvts) {
		apvts.addParameterListener(PEAKCUTOFF_ID, this);
		apvts.addParameterListener(PEAKRESO_ID, this);
		apvts.addParameterListener(PEAKVOL_ID, this);
		apvts.addParameterListener(FILTERCHOICE_ID, this);
		apvts.addParameterListener(PEAKBYPASS_ID, this);
	}

	float Process(float sample) {
		if (!bypass_) {
		}
		return 0.f;
	}
private:
	juce::AudioProcessorValueTreeState& apvts_;
	dsp::ProcessorDuplicator <dsp::IIR::Filter<float>, dsp::IIR::Coefficients <float>> filterProcessor_;
	float peakCutoff_, peakReso_, peakVol_, filterChoice_;
	bool bypass_;

	void parameterChanged(const String& id, float val) override {
		if (id == PEAKCUTOFF_ID) {
			peakCutoff_ = val;
		}
		else if (id == PEAKRESO_ID) {
			peakReso_ = val;
		}
		else if (id == PEAKVOL_ID) {
			peakVol_ = val;
		}
		else if (id == FILTERCHOICE_ID) {
			filterChoice_ = val;
		}
		else if (id == PEAKBYPASS_ID) {
			bypass_ = bool(val);
		}
	}
};