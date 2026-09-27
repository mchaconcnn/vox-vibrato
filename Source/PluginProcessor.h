#pragma once

#include <JuceHeader.h>
#include "VibratoDelay.h"

class VoxVibratoAudioProcessor final : public juce::AudioProcessor
{
public:
    VoxVibratoAudioProcessor();
    ~VoxVibratoAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;

    static constexpr auto rateId = "rate";
    static constexpr auto intensityId = "intensity";
    static constexpr auto centsId = "cents";

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    VibratoDelay vibrato;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoxVibratoAudioProcessor)
};
