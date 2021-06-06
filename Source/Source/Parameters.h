#pragma once

#define INPUT_GAIN_ID "gain"
#define INPUT_GAIN_NAME "Input Gain"
#define SINAMOUNT_ID "sinamount"
#define SINAMOUNT_NAME "Folding"
#define TANHMULT_ID "tanhmult"
#define TANHMULT_NAME "Curvature"
#define SINFREQ_ID "sinfreq"
#define SINFREQ_NAME "Fold Frequency"
#define LPCUTOFF_ID "lpcutoff"
#define LPCUTOFF_NAME "Lowpass Cutoff"
#define LPRESO_NAME "Lowpass Resonance"
#define LPRESO_ID "lpresonance"
#define OUTPUT_GAIN_ID "output"
#define OUTPUT_GAIN_NAME "Output Gain"

#define PEAKCUTOFF_ID "peakcutoff"
#define PEAKCUTOFF_NAME "Cutoff"
#define PEAKRESO_ID "peakreso"
#define PEAKRESO_NAME "Resonance"
#define PEAKVOL_ID "peakvol"
#define PEAKVOL_NAME "Peak Gain"

#define BYPASS_ID "mbypass"
#define BYPASS_NAME "Master Bypass"
#define LPBYPASS_ID "lpbypass"
#define LPBYPASS_NAME "Lowpass Bypass"
#define PEAKBYPASS_ID "peakbypass"
#define PEAKBYPASS_NAME "Filter 2 Bypass"
#define CHOICE_ID "wschoice"
#define CHOICE_NAME "Fold Wave"
#define FILTERCHOICE_ID "filterchoice"
#define FILTERCHOICE_NAME "Filter Choice"

class Parameters {
public:
	std::vector<std::unique_ptr<RangedAudioParameter>> getParameters() {
		std::vector<std::unique_ptr<RangedAudioParameter>> parameters;
        parameters.push_back(std::make_unique<AudioParameterFloat>(INPUT_GAIN_ID, INPUT_GAIN_NAME, -12.0f, 48.0f, 0.0f));
        parameters.push_back(std::make_unique<AudioParameterFloat>(SINAMOUNT_ID, SINAMOUNT_NAME, 0.0f, 100.0f, 10.0f));
        parameters.push_back(std::make_unique<AudioParameterFloat>(TANHMULT_ID, TANHMULT_NAME, 0.1f, 10.0f, 1.0f));
        parameters.push_back(std::make_unique<AudioParameterFloat>(SINFREQ_ID, SINFREQ_NAME, 5.0f, 50.0f, 20.0f));
        parameters.push_back(std::make_unique<AudioParameterFloat>(LPCUTOFF_ID, LPCUTOFF_NAME, NormalisableRange<float>(50.0f, 20000.0f, 1.0f, 0.3f, false), 20000.0f));
        parameters.push_back(std::make_unique<AudioParameterFloat>(LPRESO_ID, LPRESO_NAME, 0.1f, 0.9f, 0.5f));
        parameters.push_back(std::make_unique<AudioParameterFloat>(PEAKCUTOFF_ID, PEAKCUTOFF_NAME, NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f, false), 1000.0f));
        parameters.push_back(std::make_unique<AudioParameterFloat>(PEAKVOL_ID, PEAKVOL_NAME, NormalisableRange<float>(-48.0f, 12.0f, 0.1f, 1.5f, false), 0.0f));
        parameters.push_back(std::make_unique<AudioParameterFloat>(PEAKRESO_ID, PEAKRESO_NAME, 0.1f, 1.5f, 1.0f));
        parameters.push_back(std::make_unique<AudioParameterBool>(LPBYPASS_ID, LPBYPASS_NAME, true));
        parameters.push_back(std::make_unique<AudioParameterBool>(PEAKBYPASS_ID, PEAKBYPASS_NAME, true));
        parameters.push_back(std::make_unique<AudioParameterBool>(BYPASS_ID, BYPASS_NAME, false));
        parameters.push_back(std::make_unique<AudioParameterFloat>(CHOICE_ID, CHOICE_NAME, -1.0f, 1.0f, 0.0f));
        parameters.push_back(std::make_unique<AudioParameterFloat>(FILTERCHOICE_ID, FILTERCHOICE_NAME, 1.0f, 3.0f, 1.0f));
        parameters.push_back(std::make_unique<AudioParameterFloat>(OUTPUT_GAIN_ID, OUTPUT_GAIN_NAME, -12.0f, 3.0f, 0.0f));
        return parameters;
	}
};