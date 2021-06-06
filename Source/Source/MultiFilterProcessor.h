#pragma once
#include "Parameters.h"

struct MultiFilter : AudioProcessorValueTreeState::Listener, juce::OwnedArray<IIRFilter> {
	MultiFilter(juce::AudioProcessorValueTreeState& apvts, int numChannels) : apvts_(apvts) {
		apvts.addParameterListener(PEAKCUTOFF_ID, this);
		apvts.addParameterListener(PEAKRESO_ID, this);
		apvts.addParameterListener(PEAKVOL_ID, this);
		apvts.addParameterListener(FILTERCHOICE_ID, this);

		for (int i = 0; i < numChannels; i++) {
			this->add(new IIRFilter());
			(*this)[i]->reset();
		}
		filterChoice_ = *apvts_.getRawParameterValue(FILTERCHOICE_ID);
	}

	void setSampleRate(float sampleRate) { sampleRate_ = sampleRate; }

private:
	juce::AudioProcessorValueTreeState& apvts_;
	float sampleRate_ = 44100.f;
	float peakCutoff_ = 1000.f, peakReso_ = 1.f, peakVol_ = 1.f, filterChoice_;

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
		setFilterParameters();
	}

	void setFilterParameters() {
		if (filterChoice_ == 1) {
			for (int i = 0; i < this->size(); i++)
				(*this)[i]->setCoefficients(IIRCoefficients::makeHighPass(sampleRate_, peakCutoff_, peakReso_));
		}
		else if (filterChoice_ == 2) {
			for (int i = 0; i < this->size(); i++)
				(*this)[i]->setCoefficients(IIRCoefficients::makeNotchFilter(sampleRate_, peakCutoff_, peakReso_));
		}
		else if (filterChoice_ == 3) {
			for (int i = 0; i < this->size(); i++)
				(*this)[i]->setCoefficients(IIRCoefficients::makePeakFilter(sampleRate_, peakCutoff_, 1.f, peakVol_));
		}
	}
};