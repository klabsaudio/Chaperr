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
		peakCutoff_.setCurrentAndTargetValue(*apvts_.getRawParameterValue(PEAKCUTOFF_ID));
		peakReso_.setCurrentAndTargetValue(*apvts_.getRawParameterValue(PEAKRESO_ID));
		peakVol_.setCurrentAndTargetValue(*apvts_.getRawParameterValue(PEAKVOL_ID));

		setFilterParameters();
	}

	void reset(float sampleRate) { 
		sampleRate_ = sampleRate;

		peakCutoff_.reset(sampleRate, 0.1f);
		peakReso_.reset(sampleRate, 0.1f);
		peakVol_.reset(sampleRate, 0.1f);
	}

private:
	juce::AudioProcessorValueTreeState& apvts_;
	float sampleRate_ = 44100.f;
	float filterChoice_;
	juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> peakCutoff_, peakReso_, peakVol_;

	void parameterChanged(const String& id, float val) override {
		if (id == FILTERCHOICE_ID) {
			filterChoice_ = val;
		}
		else if (id == PEAKRESO_ID) {
			peakReso_.setTargetValue(val);

			while (peakReso_.isSmoothing()) {
				setFilterParameters();
			}
		}
		else if (id == PEAKVOL_ID) {
			peakVol_.setTargetValue(val);

			while (peakVol_.isSmoothing()) {
				setFilterParameters();
			}
		}
		else if (id == PEAKCUTOFF_ID) {
			peakCutoff_.setTargetValue(val);

			while (peakCutoff_.isSmoothing()) {
				setFilterParameters();
			}
		}
		setFilterParameters();
	}

	void setFilterParameters() {
		if (filterChoice_ == 1) {
			for (int i = 0; i < this->size(); i++)
				(*this)[i]->setCoefficients(IIRCoefficients::makeHighPass(sampleRate_, peakCutoff_.getNextValue(), peakReso_.getNextValue()));
		}
		else if (filterChoice_ == 2) {
			for (int i = 0; i < this->size(); i++)
				(*this)[i]->setCoefficients(IIRCoefficients::makeNotchFilter(sampleRate_, peakCutoff_.getNextValue(), peakReso_.getNextValue()));
		}
		else if (filterChoice_ == 3) {
			for (int i = 0; i < this->size(); i++)
				(*this)[i]->setCoefficients(IIRCoefficients::makePeakFilter(sampleRate_, peakCutoff_.getNextValue(), 1.f, peakVol_.getNextValue()));
		}
	}
};