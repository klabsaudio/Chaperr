/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WaveshaperAudioProcessorEditor::WaveshaperAudioProcessorEditor (WaveshaperAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
	setSize(700, 220);

    setResizable(false, false);
    
    //Knobs
#pragma region Knobs
    // INPUT GAIN UI SLIDER
    knobStyle(gainSlider);
    gainSlider.setRange(-12.0f, 48.0f);
    gainSlider.setTextValueSuffix(" dB");

    //SINUSOID AMOUNT
    knobStyle(sinAmountSlider);
    sinAmountSlider.setRange(0, 100);
    sinAmountSlider.setTextValueSuffix(" %");

    // TANH MULTIPLIER
    knobStyle(tanhMultSlider);
    tanhMultSlider.setRange(0.1f, 10.0f);

    //SINUSOID FREQ
    knobStyle(sinFreqSlider);
    sinFreqSlider.setRange(5.0f, 50.0f);
    sinFreqSlider.setTextValueSuffix(" Hz");

    // OUTPUT GAIN UI SLIDER
    knobStyle(waveshapeSlider);
    waveshapeSlider.setRange(-12.0f, 3.0f);
    waveshapeSlider.setTextValueSuffix(" dB");

    // LOWPASS UI SLIDERS
    knobStyle(lowPassCutoffSlider);
    lowPassCutoffSlider.setRange(50.0f, 20000.0f);
    lowPassCutoffSlider.setTextValueSuffix(" Hz");

    knobStyle(lowPassResoSlider);
    lowPassResoSlider.setRange(0.1f, 0.9f);

    lowPassButton.setColour(ToggleButton::tickColourId, Colours::white);

    // MULTI FILTER UI SLIDERS
    knobStyle(peakFreqSlider);
    peakFreqSlider.setRange(50.0f, 20000.0f);
    peakFreqSlider.setTextValueSuffix(" Hz");

    knobStyle(peakResoSlider);
    peakResoSlider.setRange(0.1f, 0.9f);

    knobStyle(peakVolumeSlider);
    peakVolumeSlider.setRange(-48.0f, 12.0f);
    peakVolumeSlider.setTextValueSuffix(" dB");

    peakBypass.setColour(ToggleButton::tickColourId, Colours::white);
#pragma endregion

    
    // WAVESHAPE CHOICE
    waveshapeChoice.addItem("Sawtooth", 1);
    waveshapeChoice.addItem("Sinewave", 2);
    waveshapeChoice.addItem("Bypass", 3);
    waveshapeChoice.setSelectedId(2);
    waveshapeChoice.setColour(ComboBox::arrowColourId, Colours::white);
    waveshapeChoice.setColour(ComboBox::backgroundColourId, Colours::transparentBlack);
    waveshapeChoice.setColour(ComboBox::outlineColourId, Colours::transparentBlack);
    waveshapeChoice.setColour(ComboBox::focusedOutlineColourId, Colours::mistyrose);

    // FILTER CHOICE
    filterChoice.addItem("LP + HP", 1);
    filterChoice.addItem("LP + Notch", 2);
    filterChoice.addItem("LP + Peak", 3);
    filterChoice.setSelectedId(2);
    filterChoice.setColour(ComboBox::arrowColourId, Colours::white);
    filterChoice.setColour(ComboBox::backgroundColourId, Colours::transparentBlack);
    filterChoice.setColour(ComboBox::outlineColourId, Colours::transparentBlack);

    //Attachments
#pragma region Attachments
    // INPUT GAIN ATTACHMENT
    gainAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTree, GAIN_ID, gainSlider));

    // OUTPUT GAIN ATTACHMENT
    waveshaperAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTree, WAVESHAPER_ID, waveshapeSlider));

    //SIN AMOUNT ATTACHMENT
    sinAmountAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTree, SINAMOUNT_ID, sinAmountSlider));

    //TANH MULTIPLIER ATTACHMENT
    tanhMultAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTree, TANHMULT_ID, tanhMultSlider));

    //SINFREQ ATTACHMENT
    sinFreqAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTree, SINFREQ_ID, sinFreqSlider));

    // LOWPASS ATTACHMENTS
    lpcutoffAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTree, LPCUTOFF_ID, lowPassCutoffSlider));
    lpresonanceAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTree, LPRESO_ID, lowPassResoSlider));

    // PEAK ATTACHMENTS
    peakFrqAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTree, PEAKCUTOFF_ID, peakFreqSlider));
    peakResoAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTree, PEAKRESO_ID, peakResoSlider));
    peakVolumeAttachment.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.valueTree, PEAKVOL_ID, peakVolumeSlider));

    // BYPASS BUTTONS ATTACHMENTS
    lowPassBypassAttachment.reset(new AudioProcessorValueTreeState::ButtonAttachment(processor.valueTree, LPBYPASS_ID, lowPassButton));
    peakBypassAttachment.reset(new AudioProcessorValueTreeState::ButtonAttachment(processor.valueTree, PEAKBYPASS_ID, peakBypass));
    masterBypassAttachment.reset(new AudioProcessorValueTreeState::ButtonAttachment(processor.valueTree, BYPASS_ID, masterBypassButton));

    // CHOICEBOX ATTACHMENTS
    waveshapeChoiceAttachment.reset(new AudioProcessorValueTreeState::ComboBoxAttachment(processor.valueTree, CHOICE_ID, waveshapeChoice));
    filterChoiceAttachment.reset(new AudioProcessorValueTreeState::ComboBoxAttachment(processor.valueTree, FILTERCHOICE_ID, filterChoice));

#pragma endregion

    // MAKE UI VISIBLE

    addAndMakeVisible(&peakBypass);
    addAndMakeVisible(&lowPassButton);
    addAndMakeVisible(&masterBypassButton);
    addAndMakeVisible(&waveshapeChoice);
    addAndMakeVisible(&filterChoice);
    
}

WaveshaperAudioProcessorEditor::~WaveshaperAudioProcessorEditor()
{
}

void WaveshaperAudioProcessorEditor::knobStyle(juce::Slider& knob)
{
    //Colours
    knob.setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
    knob.setColour(Slider::thumbColourId, Colours::transparentBlack);
    knob.setColour(Slider::rotarySliderFillColourId, Colours::white);
    knob.setColour(Slider::rotarySliderOutlineColourId, Colours::transparentBlack);

    //Parameters
    knob.setSliderStyle(Slider::SliderStyle::Rotary);
    knob.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 30);
    knob.setTextBoxIsEditable(true);

    addAndMakeVisible(&knob);
}

//==============================================================================
void WaveshaperAudioProcessorEditor::paint (Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    // BACKGROUND IMAGE
    //Image background = ImageCache::getFromMemory(BinaryData::waveshaper_background_png, BinaryData::waveshaper_background_pngSize);
    //g.drawImageAt(background, 0, 0);

    //backgroundImage = Drawable::createFromImageData(BinaryData::chaperr_background_svg, BinaryData::chaperr_background_svgSize);
    //backgroundImage->setBounds(getLocalBounds());

    back = XmlDocument::parse(BinaryData::chaperr_background_svg);
    //logo = XmlDocument::parse(BinaryData::logo_svg);

    std::unique_ptr<Drawable> svg_drawable_play = Drawable::createFromSVG(*back);
    //std::unique_ptr<Drawable> svg_drawable_logo = Drawable::createFromSVG(*logo);

    svg_drawable_play->setTransformToFit(Rectangle<float>(0, 0, 700, 220), RectanglePlacement::stretchToFit);
    svg_drawable_play->draw(g, 1.0f); //juce::AffineTransform::scale(3.78));

    //svg_drawable_logo->setTransformToFit(Rectangle<float>(605, 100, 80, 100), RectanglePlacement::centred);
    //svg_drawable_logo->draw(g, 1.0f); //juce::AffineTransform::scale(3.78));

    if(filterChoice.getSelectedId() == 3)
    {
        peakVolumeSlider.setVisible(true);
        peakResoSlider.setVisible(false);
    }
    else
    {
        peakVolumeSlider.setVisible(false);
        peakResoSlider.setVisible(true);
    }
    
    // TEXT COLOUR & SIZE
    g.setColour (Colours::white);
    g.setFont (15.0f);
}

void WaveshaperAudioProcessorEditor::resized()
{
    // SLIDERS POSITIONS
    gainSlider.setBounds(0, 0, 200, 200);
    sinAmountSlider.setBounds(200, 0, 100, 100);
    tanhMultSlider.setBounds(200, 100, 100, 100);
    sinFreqSlider.setBounds(300, 0, 100, 100);
    waveshapeSlider.setBounds(600, 0, 100, 100);
    lowPassCutoffSlider.setBounds(400, 100, 100, 100);
    lowPassResoSlider.setBounds(400, 0, 100, 100);
    peakFreqSlider.setBounds(500, 100, 100, 100);
    peakResoSlider.setBounds(500, 0, 100, 100);
    peakVolumeSlider.setBounds(500, 0, 100, 100);
    
    // BUTTONS POSITIONS
    lowPassButton.setBounds(435, 197, 85, 20);
    peakBypass.setBounds(535, 197, 85, 20);
    masterBypassButton.setBounds(15, 197, 185, 20);
    
    // CHOICEBOX POSITION
    waveshapeChoice.setBounds(310, 123, 100, 30); // was 143
    filterChoice.setBounds(310, 172, 100, 30);
}
