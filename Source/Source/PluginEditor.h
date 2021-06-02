#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class WaveshaperAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    WaveshaperAudioProcessorEditor (WaveshaperAudioProcessor&);
    ~WaveshaperAudioProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;
    void knobStyle(juce::Slider& knob);
    
    Slider gainSlider;
    Slider waveshapeSlider;

    Slider peakFreqSlider;
    Slider peakResoSlider;
    Slider peakVolumeSlider;

    Slider lowPassCutoffSlider;
    Slider lowPassResoSlider;

    Slider sinAmountSlider;
    Slider tanhMultSlider;
    Slider sinFreqSlider;

private:
    
    WaveshaperAudioProcessor& processor;
    int yAxis = 0;
    
    ToggleButton lowPassButton;
    ToggleButton peakBypass;
    ToggleButton masterBypassButton;

    Label inputLabel;
    Label masterBypassLabel;
    Label foldingLabel;
    Label curvatureLabel;
    Label foldFreqLabel;
    Label foldChoiceLabel;
    Label filterChoiceLabel;
    
    ComboBox waveshapeChoice;
    ComboBox filterChoice;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperAudioProcessorEditor)

public:
    
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> waveshaperAttachment;
    
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> sinAmountAttachment;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> tanhMultAttachment;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> sinFreqAttachment;
    
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> peakFrqAttachment;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> peakResoAttachment;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> peakVolumeAttachment;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> lpcutoffAttachment;
    std::unique_ptr <AudioProcessorValueTreeState::SliderAttachment> lpresonanceAttachment;
    
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> masterBypassAttachment;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> lowPassBypassAttachment;
    std::unique_ptr <AudioProcessorValueTreeState::ButtonAttachment> peakBypassAttachment;
    
    std::unique_ptr <AudioProcessorValueTreeState::ComboBoxAttachment> waveshapeChoiceAttachment;
    std::unique_ptr <AudioProcessorValueTreeState::ComboBoxAttachment> filterChoiceAttachment;

    ScopedPointer<Drawable> backgroundImage;

    std::unique_ptr <XmlElement> back;
    std::unique_ptr <XmlElement> logo;
};
