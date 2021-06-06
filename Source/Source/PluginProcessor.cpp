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
	mf_(valueTree, getNumInputChannels()), lpf_(valueTree, getNumInputChannels())
#endif
{
	valueTree.addParameterListener(BYPASS_ID, this);
	valueTree.addParameterListener(INPUT_GAIN_ID, this);
	valueTree.addParameterListener(OUTPUT_GAIN_ID, this);
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

	lpf_.setSampleRate(sampleRate);
	mf_.setSampleRate(sampleRate);
}

void WaveshaperAudioProcessor::parameterChanged(const String& id, float val) {
	if (id == BYPASS_ID) {
		masterBypass_ = bool(val);
	}
	else if (id == INPUT_GAIN_ID) {
		inputGain_ = val;
	}
	else if (id == OUTPUT_GAIN_ID) {
		outputGain_ = val;
	}
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

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
		buffer.clear(i, 0, buffer.getNumSamples());
	}

	if (!masterBypass_) {
		for (int chan = 0; chan < totalNumInputChannels; ++chan) {
			auto chanBuffer = buffer.getWritePointer(chan);
			for (auto s = 0; s < buffer.getNumSamples(); ++s) {
				chanBuffer[s] *= Decibels::decibelsToGain(inputGain_);

				//Waveshape & Wavefold process
				//sample[s] = wavefoldProcess(sample[s]);

				chanBuffer[s] = lpf_[chan]->processSingleSampleRaw(chanBuffer[s]);
				chanBuffer[s] = mf_[chan]->processSingleSampleRaw(chanBuffer[s]);

				//Output Gain
				chanBuffer[s] *= Decibels::decibelsToGain(outputGain_);
			}
		}
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
