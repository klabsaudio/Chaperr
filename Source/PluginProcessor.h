/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#define GAIN_ID "gain"
#define GAIN_NAME "Input Gain"
#define SINAMOUNT_ID "sinamount"
#define SINAMOUNT_NAME "Folding"
#define TANHMULT_ID "tanhmult"
#define TANHMULT_NAME "Curvature"
#define SINFREQ_ID "sinfreq"
#define SINFREQ_NAME "Fold Frequency"
#define WAVESHAPER_ID "waveshaper"
#define WAVESHAPER_NAME "Output Gain"
#define LPCUTOFF_ID "lpcutoff"
#define LPCUTOFF_NAME "Lowpass Cutoff"
#define LPRESO_NAME "Lowpass Resonance"
#define LPRESO_ID "lpresonance"

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

//==============================================================================
/**
*/
class WaveshaperAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    WaveshaperAudioProcessor();
    ~WaveshaperAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    double lastSampleRate;
    float wavefoldProcess(float sampleToProcess);
    void LowPassFilter(dsp::AudioBlock<float> bufferBlock);
    void MultiFilter(dsp::AudioBlock<float> bufferBlock);
    float GaussianSmooth(float x);
    
    AudioProcessorValueTreeState valueTree;
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
private:
    
    enum
    {
        gainIndex,
        waveshaperIndex,
        ladderLPIndex
    };
    
    /*juce::dsp::ProcessorChain<
        juce::dsp::Gain<float>,
        juce::dsp::WaveShaper<double>,
        juce::dsp::LadderFilter<float>
    > processorChain;*/

    dsp::ProcessorDuplicator <dsp::IIR::Filter<float>, dsp::IIR::Coefficients <float>> lowpassProcessor;
    dsp::ProcessorDuplicator <dsp::IIR::Filter<float>, dsp::IIR::Coefficients <float>> multiFilterProcessor;

    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> cutoffValue, gainValue, peakValue;

    float piValue;
    float phase;
    float cutoffPrev, gainPrev, peakPrev, waveshapeChoicePrev;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperAudioProcessor)
};
