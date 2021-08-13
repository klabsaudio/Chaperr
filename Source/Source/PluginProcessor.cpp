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
	apvts_(*this, nullptr, "Parameters", createParameterLayout()),
	mf_(apvts_, getNumInputChannels()), lpf_(apvts_, getNumInputChannels()),
	wsp_(apvts_)
#endif
{
	apvts_.addParameterListener(BYPASS_ID, this);
	apvts_.addParameterListener(INPUT_GAIN_ID, this);
	apvts_.addParameterListener(OUTPUT_GAIN_ID, this);
	apvts_.addParameterListener(LPBYPASS_ID, this);
	apvts_.addParameterListener(PEAKBYPASS_ID, this);

	masterBypass_ = *apvts_.getRawParameterValue(BYPASS_ID);
	inputGain_.setTargetValue(*apvts_.getRawParameterValue(INPUT_GAIN_ID));
	outputGain_.setTargetValue(*apvts_.getRawParameterValue(OUTPUT_GAIN_ID));
	lpfBypass_ = *apvts_.getRawParameterValue(LPBYPASS_ID);
	mfBypass_ = *apvts_.getRawParameterValue(PEAKBYPASS_ID);
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

	lpf_.reset(sampleRate);
	mf_.reset(sampleRate);

	inputGain_.reset(sampleRate, 0.25f);
	outputGain_.reset(sampleRate, 0.25f);
}

void WaveshaperAudioProcessor::parameterChanged(const String& id, float val) {
	if (id == BYPASS_ID) {
		masterBypass_ = bool(val);
	}
	else if (id == INPUT_GAIN_ID) {
		inputGain_.setTargetValue(val);
	}
	else if (id == OUTPUT_GAIN_ID) {
		outputGain_.setTargetValue(val);
	}
	else if (id == LPBYPASS_ID) {
		lpfBypass_ = bool(val);
	}
	else if (id == PEAKBYPASS_ID) {
		mfBypass_ = bool(val);
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
				chanBuffer[s] *= Decibels::decibelsToGain(inputGain_.getNextValue());

				//Waveshape & Wavefold process
				chanBuffer[s] = wsp_.Process(chanBuffer[s]);

				if(!lpfBypass_)
					chanBuffer[s] = lpf_.Process(chan, chanBuffer[s]);
				if(!mfBypass_)
					chanBuffer[s] = mf_[chan]->processSingleSampleRaw(chanBuffer[s]);

				//Output Gain
				chanBuffer[s] *= Decibels::decibelsToGain(outputGain_.getNextValue());
			}
		}
	}
}

bool WaveshaperAudioProcessor::hasEditor() const {
	return true;
}

AudioProcessorEditor* WaveshaperAudioProcessor::createEditor() {
	return new WaveshaperAudioProcessorEditor(*this);
}

//==============================================================================
void WaveshaperAudioProcessor::getStateInformation(MemoryBlock& destData) {
	auto state = apvts_.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void WaveshaperAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(apvts_.state.getType()))
			apvts_.replaceState(juce::ValueTree::fromXml(*xmlState));
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
	return new WaveshaperAudioProcessor();
}
