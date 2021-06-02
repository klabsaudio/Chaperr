/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WaveshaperAudioProcessor::WaveshaperAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ), valueTree(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

WaveshaperAudioProcessor::~WaveshaperAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout WaveshaperAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> parameters;
    
    parameters.push_back(std::make_unique<AudioParameterFloat>(GAIN_ID, GAIN_NAME, -12.0f, 48.0f, 0.0f));
    
    parameters.push_back(std::make_unique<AudioParameterFloat>(SINAMOUNT_ID, SINAMOUNT_NAME, 0.0f, 100.0f, 10.0f));
    
    parameters.push_back(std::make_unique<AudioParameterFloat>(TANHMULT_ID, TANHMULT_NAME, 0.1f, 10.0f, 1.0f));
    
    parameters.push_back(std::make_unique<AudioParameterFloat>(SINFREQ_ID, SINFREQ_NAME, 5.0f, 50.0f, 20.0f));
    
    parameters.push_back(std::make_unique<AudioParameterFloat>(WAVESHAPER_ID, WAVESHAPER_NAME, -12.0f, 3.0f, 0.0f));
    
    parameters.push_back(std::make_unique<AudioParameterFloat>(LPCUTOFF_ID, LPCUTOFF_NAME, NormalisableRange<float> (50.0f, 20000.0f, 1.0f, 0.3f, false), 20000.0f));
    
    parameters.push_back(std::make_unique<AudioParameterFloat>(LPRESO_ID, LPRESO_NAME, 0.1f, 0.9f, 0.5f));
    
    parameters.push_back(std::make_unique<AudioParameterFloat>(PEAKCUTOFF_ID, PEAKCUTOFF_NAME, NormalisableRange<float> (20.0f, 20000.0f, 1.0f, 0.3f, false), 1000.0f));

    parameters.push_back(std::make_unique<AudioParameterFloat>(PEAKVOL_ID, PEAKVOL_NAME, NormalisableRange<float>(-48.0f, 12.0f, 0.1f, 1.5f, false), 0.0f));
    
    parameters.push_back(std::make_unique<AudioParameterFloat>(PEAKRESO_ID, PEAKRESO_NAME, 0.1f, 1.5f, 1.0f));
    
    parameters.push_back(std::make_unique<AudioParameterBool>(LPBYPASS_ID, LPBYPASS_NAME, true));
    parameters.push_back(std::make_unique<AudioParameterBool>(PEAKBYPASS_ID, PEAKBYPASS_NAME, true));
    parameters.push_back(std::make_unique<AudioParameterBool>(BYPASS_ID, BYPASS_NAME, false));
    
    parameters.push_back(std::make_unique<AudioParameterFloat>(CHOICE_ID, CHOICE_NAME, -1.0f, 1.0f, 0.0f));
    parameters.push_back(std::make_unique<AudioParameterFloat>(FILTERCHOICE_ID, FILTERCHOICE_NAME, 1.0f, 3.0f, 1.0f));
    
    return {parameters.begin(), parameters.end()};
}

//==============================================================================
const String WaveshaperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WaveshaperAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WaveshaperAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WaveshaperAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WaveshaperAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WaveshaperAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WaveshaperAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WaveshaperAudioProcessor::setCurrentProgram (int index)
{
}

const String WaveshaperAudioProcessor::getProgramName (int index)
{
    return {};
}

void WaveshaperAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void WaveshaperAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    lastSampleRate = sampleRate;
    dsp::ProcessSpec spec;
    spec.sampleRate = lastSampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    //processorChain.reset();
    //processorChain.prepare(spec);
    //processorChain.template get<ladderLPIndex>().setMode(juce::dsp::LadderFilter<float>::Mode::LPF24);

    lowpassProcessor.prepare(spec);
    lowpassProcessor.reset();
    multiFilterProcessor.prepare(spec);
    multiFilterProcessor.reset();

    cutoffValue.reset(lastSampleRate, 0.001f);
    cutoffValue.reset(4);
    cutoffPrev = *valueTree.getRawParameterValue(LPCUTOFF_ID);
    cutoffValue.setCurrentAndTargetValue(cutoffPrev);

    gainValue.reset(lastSampleRate, 0.001f);
    gainValue.reset(2);
    gainPrev = *valueTree.getRawParameterValue(GAIN_ID);
    gainValue.setCurrentAndTargetValue(gainPrev);

    peakValue.reset(lastSampleRate, 0.001f);
    peakValue.reset(4);
    peakPrev = *valueTree.getRawParameterValue(PEAKCUTOFF_ID);
    peakValue.setCurrentAndTargetValue(peakPrev);
}

void WaveshaperAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WaveshaperAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


void WaveshaperAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    //Get bypass parameters from GUI
    bool masterBypassBool = *valueTree.getRawParameterValue(BYPASS_ID);
    bool lowPassBypassBool = *valueTree.getRawParameterValue(LPBYPASS_ID);
    bool highPassBypassBool = *valueTree.getRawParameterValue(PEAKBYPASS_ID);

    //Get volume parameters from GUI
    float inputGainValue = *valueTree.getRawParameterValue(GAIN_ID);
    float outputGainValue = *valueTree.getRawParameterValue(WAVESHAPER_ID);
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
    }

    if (!masterBypassBool) {

        for (int channel = 0; channel < totalNumInputChannels; ++channel) {
            for (auto s = 0; s < buffer.getNumSamples(); ++s) {
                auto sample = buffer.getWritePointer(channel);
                sample[s] *= Decibels::decibelsToGain(inputGainValue);

                //Waveshape & Wavefold process
                sample[s] = wavefoldProcess(sample[s]);

                //Output Gain
                sample[s] *= Decibels::decibelsToGain(outputGainValue);
            }
        }

        dsp::AudioBlock<float> block(buffer);

        //Lowpass filter process
        if(!lowPassBypassBool) {
            lowPassFilter_(block);
        }

        //Multi filter process
        if (!highPassBypassBool) {
            multiFilter_(block);
        }
    }
}

float WaveshaperAudioProcessor::wavefoldProcess(float sampleToProcess)
{
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
    } else if (waveshapeChoice_ == 0) {
        //Sinewave
        sampleToProcess += sinAmountFloat_ * std::sin(memorySample_ * sinFreqValue_);
    }

    sampleToProcess = juce::jlimit(float (-1.0), float (1.0), sampleToProcess);

    return sampleToProcess;
}

void WaveshaperAudioProcessor::multiFilter_(dsp::AudioBlock<float> bufferBlock)
{
    //Get parameters from GUI
    float filterChoice = *valueTree.getRawParameterValue(FILTERCHOICE_ID);
    float highPassCutoffValue = *valueTree.getRawParameterValue(PEAKCUTOFF_ID);
    float highPassResoValue = *valueTree.getRawParameterValue(PEAKRESO_ID);
    float peakVolume = *valueTree.getRawParameterValue(PEAKVOL_ID);

    //Smooth frequency knob
    if (highPassCutoffValue != peakPrev)
    {
        peakValue.setTargetValue(highPassCutoffValue);
        peakPrev = peakValue.getNextValue();
    }

	//Set filter type
	switch ((int)filterChoice)
	{
	case 1: //Highpass Filter
		*multiFilterProcessor.state = *dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate, peakPrev, highPassResoValue);
		break;
	case 2: //Notch Filter
		*multiFilterProcessor.state = *dsp::IIR::Coefficients<float>::makeNotch(lastSampleRate, peakPrev, highPassResoValue);
		break;
	case 3: //Peak Filter
		*multiFilterProcessor.state = *dsp::IIR::Coefficients<float>::makePeakFilter(lastSampleRate, peakPrev, 1.5, Decibels::decibelsToGain(peakVolume));
		break;
	}

    //Process the block
    multiFilterProcessor.process(dsp::ProcessContextReplacing<float>(bufferBlock));
}

void WaveshaperAudioProcessor::lowPassFilter_(dsp::AudioBlock<float> bufferBlock)
{
    //Get parameters from GUI
    float lowPassCutoffValue = *valueTree.getRawParameterValue(LPCUTOFF_ID);
    float lowPassResoValue = *valueTree.getRawParameterValue(LPRESO_ID);

    //Smooth frequency knob
    if (lowPassCutoffValue != cutoffPrev)
    {
        cutoffValue.setTargetValue(lowPassCutoffValue);
        cutoffPrev = cutoffValue.getNextValue();
    }

    //Update cutoff and Q
    *lowpassProcessor.state = *dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, cutoffPrev, lowPassResoValue);
    // Process the block
    lowpassProcessor.process(dsp::ProcessContextReplacing<float>(bufferBlock));
}

//==============================================================================
bool WaveshaperAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* WaveshaperAudioProcessor::createEditor()
{
    return new WaveshaperAudioProcessorEditor (*this);
}

//==============================================================================
void WaveshaperAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto state = valueTree.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void WaveshaperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(valueTree.state.getType()))
            valueTree.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WaveshaperAudioProcessor();
}
