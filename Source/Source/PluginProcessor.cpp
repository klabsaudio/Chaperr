#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"

WaveshaperAudioProcessor::WaveshaperAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
	valueTree(*this, nullptr, "Parameters", createParameterLayout()),
	multiFilters_(valueTree, getNumInputChannels())
#endif
{
	valueTree.addParameterListener(PEAKCUTOFF_ID, this);
}

WaveshaperAudioProcessor::~WaveshaperAudioProcessor() {}

AudioProcessorValueTreeState::ParameterLayout WaveshaperAudioProcessor::createParameterLayout() {
	Parameters p;
	std::vector<std::unique_ptr<RangedAudioParameter>> parameters = p.getParameters();
	return { parameters.begin(), parameters.end() };
}

//==============================================================================
const String WaveshaperAudioProcessor::getName() const {
	return JucePlugin_Name;
}

bool WaveshaperAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool WaveshaperAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool WaveshaperAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double WaveshaperAudioProcessor::getTailLengthSeconds() const {
	return 0.0;
}

int WaveshaperAudioProcessor::getNumPrograms() {
	return 1;
}

int WaveshaperAudioProcessor::getCurrentProgram() {
	return 0;
}

void WaveshaperAudioProcessor::setCurrentProgram(int index) {}

const String WaveshaperAudioProcessor::getProgramName(int index) {
	return {};
}

void WaveshaperAudioProcessor::changeProgramName(int index, const String& newName)
{
}
//==============================================================================
void WaveshaperAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
	lastSampleRate = sampleRate;
	dsp::ProcessSpec spec;
	spec.sampleRate = lastSampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getTotalNumOutputChannels();

	for (auto f : multiFilters_)
		f->setSampleRate(sampleRate);

	//lowpassProcessor.prepare(spec);
	//lowpassProcessor.reset();
	//multiFilterProcessor.prepare(spec);
	//multiFilterProcessor.reset();

	//cutoffValue.reset(lastSampleRate, 0.001f);
	//cutoffValue.reset(4);
	//cutoffPrev = *valueTree.getRawParameterValue(LPCUTOFF_ID);
	//cutoffValue.setCurrentAndTargetValue(cutoffPrev);

	//gainValue.reset(lastSampleRate, 0.001f);
	//gainValue.reset(2);
	//gainPrev = *valueTree.getRawParameterValue(GAIN_ID);
	//gainValue.setCurrentAndTargetValue(gainPrev);

	//peakValue.reset(lastSampleRate, 0.001f);
	//peakValue.reset(4);
	//peakPrev = *valueTree.getRawParameterValue(PEAKCUTOFF_ID);
	//peakValue.setCurrentAndTargetValue(peakPrev);
}

void WaveshaperAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WaveshaperAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;

#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void WaveshaperAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) {
	ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	//Get bypass parameters from GUI
	bool masterBypassBool = *valueTree.getRawParameterValue(BYPASS_ID);
	//bool lowPassBypassBool = *valueTree.getRawParameterValue(LPBYPASS_ID);

	//Get volume parameters from GUI
	float inputGainValue = *valueTree.getRawParameterValue(GAIN_ID);
	float outputGainValue = *valueTree.getRawParameterValue(WAVESHAPER_ID);

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
		buffer.clear(i, 0, buffer.getNumSamples());
	}

	if (!masterBypassBool) {
		for (int chan = 0; chan < totalNumInputChannels; ++chan) {
			auto chanBuffer = buffer.getWritePointer(chan);
			for (auto s = 0; s < buffer.getNumSamples(); ++s) {
				chanBuffer[s] *= Decibels::decibelsToGain(inputGainValue);

				//Waveshape & Wavefold process
				//sample[s] = wavefoldProcess(sample[s]);

				chanBuffer[s] = multiFilters_[chan]->Process(chanBuffer[s]);

				//Output Gain
				chanBuffer[s] *= Decibels::decibelsToGain(outputGainValue);
			}
		}

		//dsp::AudioBlock<float> block(buffer);

		//Lowpass filter process
		//if(!lowPassBypassBool) {
		//    lowPassFilter_(block);
		//}
	}
}

float WaveshaperAudioProcessor::wavefoldProcess(float sampleToProcess) {
	//Get parameters from GUI
	float waveshapeChoice_ = *valueTree.getRawParameterValue(CHOICE_ID);
	float tanhMultValue_ = *valueTree.getRawParameterValue(TANHMULT_ID);
	float sinFreqValue_ = *valueTree.getRawParameterValue(SINFREQ_ID);
	float sinAmountFloat_ = *valueTree.getRawParameterValue(SINAMOUNT_ID) * 0.005f;
	float memorySample_ = sampleToProcess;

	//Tanh function applied to the signal
	sampleToProcess = std::tanh(sampleToProcess * tanhMultValue_);

	if (waveshapeChoice_ == -1) {
		//Triangle (not really a pure triangle)
		sampleToProcess += sinAmountFloat_ * std::asin(std::sin(memorySample_ * (sinFreqValue_ - 4.f)));
		sampleToProcess += sinAmountFloat_ * std::sin(memorySample_ * sinFreqValue_ * 2.f);
	}
	else if (waveshapeChoice_ == 0) {
		//Sinewave
		sampleToProcess += sinAmountFloat_ * std::sin(memorySample_ * sinFreqValue_);
	}

	sampleToProcess = juce::jlimit(float(-1.0), float(1.0), sampleToProcess);

	return sampleToProcess;
}

void WaveshaperAudioProcessor::lowPassFilter_(dsp::AudioBlock<float> bufferBlock) {
	//Get parameters from GUI
	float lowPassCutoffValue = *valueTree.getRawParameterValue(LPCUTOFF_ID);
	float lowPassResoValue = *valueTree.getRawParameterValue(LPRESO_ID);

	//Smooth frequency knob
	//if (lowPassCutoffValue != cutoffPrev)
	//{
	//    cutoffValue.setTargetValue(lowPassCutoffValue);
	//    cutoffPrev = cutoffValue.getNextValue();
	//}

	//Update cutoff and Q
	//*lowpassProcessor.state = *dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, cutoffPrev, lowPassResoValue);
	// Process the block
	lowpassProcessor.process(dsp::ProcessContextReplacing<float>(bufferBlock));
}

bool WaveshaperAudioProcessor::hasEditor() const {
	return true;
}

AudioProcessorEditor* WaveshaperAudioProcessor::createEditor() {
	return new WaveshaperAudioProcessorEditor(*this);
}

//==============================================================================
void WaveshaperAudioProcessor::getStateInformation(MemoryBlock& destData) {
	auto state = valueTree.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void WaveshaperAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(valueTree.state.getType()))
			valueTree.replaceState(juce::ValueTree::fromXml(*xmlState));
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
	return new WaveshaperAudioProcessor();
}
