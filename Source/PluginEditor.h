#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class VoxLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    VoxLookAndFeel();
    void drawRotarySlider (juce::Graphics&, int, int, int, int, float,
                           float, float, juce::Slider&) override;
    void drawLinearSlider (juce::Graphics&, int, int, int, int, float, float, float,
                           juce::Slider::SliderStyle, juce::Slider&) override;
};

class WaveformDisplay final : public juce::Component,
                              private juce::Timer
{
public:
    explicit WaveformDisplay (VoxVibratoAudioProcessor&);
    void paint (juce::Graphics&) override;

private:
    void timerCallback() override { repaint(); }
    VoxVibratoAudioProcessor& processor;
};

class VoxVibratoAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit VoxVibratoAudioProcessorEditor (VoxVibratoAudioProcessor&);
    ~VoxVibratoAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    void configureSlider (juce::Slider&, juce::Label&, const juce::String& title,
                          const juce::String& suffix);

    VoxLookAndFeel lookAndFeel;
    WaveformDisplay waveform;
    juce::Slider rateSlider;
    juce::Slider intensitySlider;
    juce::Slider centsSlider;
    juce::Label rateLabel;
    juce::Label intensityLabel;
    juce::Label centsLabel;
    juce::HyperlinkButton website { "rco.cr/vox/vibrato/", juce::URL ("https://rco.cr/vox/vibrato/") };
    std::unique_ptr<SliderAttachment> rateAttachment;
    std::unique_ptr<SliderAttachment> intensityAttachment;
    std::unique_ptr<SliderAttachment> centsAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoxVibratoAudioProcessorEditor)
};
