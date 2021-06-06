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
    float wavefoldProcess(float sampleToProcess);
    
    AudioProcessorValueTreeState valueTree;
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
private:
    LowPassFilter lpf_;
    MultiFilter mf_;
    WaveshapeProcessor wsp_;

    void parameterChanged(const String& id, float val) override;

    enum {
        gainIndex,
        waveshaperIndex,
        ladderLPIndex
    };

    static constexpr float pi_ = juce::MathConstants<float>::pi;
    static constexpr float twoPi_ = juce::MathConstants<float>::twoPi;
    float inputGain_, outputGain_;
    bool masterBypass_, lpfBypass_, mfBypass_;
    float phase;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperAudioProcessor)
};
