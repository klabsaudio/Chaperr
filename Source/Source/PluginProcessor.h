#pragma once

#include <JuceHeader.h>
#include "MultiFilterProcessor.h"
#include "LowpassProcessor.h"
#include "WaveshapeProcessor.h"

class WaveshaperAudioProcessor  : public AudioProcessor, public AudioProcessorValueTreeState::Listener {
public:
    WaveshaperAudioProcessor();
    ~WaveshaperAudioProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    double lastSampleRate;
    
    AudioProcessorValueTreeState apvts_;
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
private:
    LowPassFilter lpf_;
    MultiFilter mf_;
    WaveshapeProcessor wsp_;

    void parameterChanged(const String& id, float val) override;

    SmoothedValue<float, juce::ValueSmoothingTypes::Linear> inputGain_, outputGain_;
    bool masterBypass_, lpfBypass_, mfBypass_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperAudioProcessor)
};
