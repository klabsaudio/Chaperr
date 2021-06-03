#pragma once

class Parameters {
public:
	std::vector<std::unique_ptr<RangedAudioParameter>> getParameters() {
		std::vector<std::unique_ptr<RangedAudioParameter>> parameters;
        parameters.push_back(std::make_unique<AudioParameterFloat>(GAIN_ID, GAIN_NAME, -12.0f, 48.0f, 0.0f));
        parameters.push_back(std::make_unique<AudioParameterFloat>(SINAMOUNT_ID, SINAMOUNT_NAME, 0.0f, 100.0f, 10.0f));
        parameters.push_back(std::make_unique<AudioParameterFloat>(TANHMULT_ID, TANHMULT_NAME, 0.1f, 10.0f, 1.0f));
        parameters.push_back(std::make_unique<AudioParameterFloat>(SINFREQ_ID, SINFREQ_NAME, 5.0f, 50.0f, 20.0f));
        parameters.push_back(std::make_unique<AudioParameterFloat>(WAVESHAPER_ID, WAVESHAPER_NAME, -12.0f, 3.0f, 0.0f));
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
        return parameters;
	}
};