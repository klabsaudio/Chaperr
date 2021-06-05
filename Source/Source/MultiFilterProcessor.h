#pragma once
#include "Parameters.h"

struct MultiFilter : AudioProcessorValueTreeState::Listener {
	MultiFilter(juce::AudioProcessorValueTreeState& apvts) : apvts_(apvts) {
		apvts.addParameterListener(PEAKCUTOFF_ID, this);
		apvts.addParameterListener(PEAKRESO_ID, this);
		apvts.addParameterListener(PEAKVOL_ID, this);
		apvts.addParameterListener(FILTERCHOICE_ID, this);
		apvts.addParameterListener(PEAKBYPASS_ID, this);
		filterProcessor_.reset();
		filterChoice_ = *apvts_.getRawParameterValue(FILTERCHOICE_ID);
	}

	float Process(float sample) {
		if (!bypass_) {
			return filterProcessor_.processSingleSampleRaw(sample);
		}
		return sample;
	}

	void setSampleRate(float sampleRate) { sampleRate_ = sampleRate; }

private:
	juce::AudioProcessorValueTreeState& apvts_;
	IIRFilter filterProcessor_;
	float sampleRate_ = 44100.f;
	float peakCutoff_ = 1000.f, peakReso_ = 1.f, peakVol_ = 1.f, filterChoice_;
	bool bypass_;

	void parameterChanged(const String& id, float val) override {
		if (id == FILTERCHOICE_ID) {
			filterChoice_ = val;
		}
		else if (id == PEAKRESO_ID) {
			peakReso_ = val;
		}
		else if (id == PEAKVOL_ID) {
			peakVol_ = val;
		}
		else if (id == PEAKCUTOFF_ID) {
			peakCutoff_ = val;
		}
		else if (id == PEAKBYPASS_ID) {
			bypass_ = bool(val);
		}
		setFilterParameters();
	}

	void setFilterParameters() {
		if (filterChoice_ == 1) {
			filterProcessor_.setCoefficients(IIRCoefficients::makeHighPass(sampleRate_, peakCutoff_, peakReso_));
		}
		else if (filterChoice_ == 2) {
			filterProcessor_.setCoefficients(IIRCoefficients::makeNotchFilter(sampleRate_, peakCutoff_, peakReso_));
		}
		else if (filterChoice_ == 3) {
			filterProcessor_.setCoefficients(IIRCoefficients::makePeakFilter(sampleRate_, peakCutoff_, 1.f, peakVol_));
		}
	}
};