#include "PluginProcessor.h"
#include "PluginEditor.h"

VoxVibratoAudioProcessor::VoxVibratoAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "VoxVibratoState", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout VoxVibratoAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { rateId, 1 }, "Rate",
        juce::NormalisableRange<float> { 0.0f, 10.0f, 0.01f }, 5.0f,
        juce::AudioParameterFloatAttributes().withLabel ("Hz")));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { intensityId, 1 }, "Intensity",
        juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1f }, 50.0f,
        juce::AudioParameterFloatAttributes().withLabel ("%")));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { centsId, 1 }, "Peak Shift",
        juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1f }, 20.0f,
        juce::AudioParameterFloatAttributes().withLabel ("cents")));

    return layout;
}

void VoxVibratoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    vibrato.prepare (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void VoxVibratoAudioProcessor::releaseResources()
{
    vibrato.reset();
}

bool VoxVibratoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& output = layouts.getMainOutputChannelSet();
    return (output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo())
        && output == layouts.getMainInputChannelSet();
}

void VoxVibratoAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    for (auto channel = getTotalNumInputChannels(); channel < getTotalNumOutputChannels(); ++channel)
        buffer.clear (channel, 0, buffer.getNumSamples());

    vibrato.setTargets (parameters.getRawParameterValue (rateId)->load(),
                        parameters.getRawParameterValue (intensityId)->load(),
                        parameters.getRawParameterValue (centsId)->load());
    vibrato.process (buffer);
}

void VoxVibratoAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = parameters.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void VoxVibratoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* VoxVibratoAudioProcessor::createEditor()
{
    return new VoxVibratoAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VoxVibratoAudioProcessor();
}
