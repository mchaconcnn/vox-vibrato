#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <algorithm>
#include <cmath>
#include <vector>

/**
    A stereo-coherent, continuously modulated fractional delay.

    A changing delay produces a changing playback rate. The delay excursion is
    derived from the requested peak cents so the control remains musically
    meaningful at normal vocal-vibrato rates. Below 0.05 Hz the excursion is
    bounded to keep latency practical; 0 Hz intentionally becomes a neutral,
    static delay.
*/
class VibratoDelay
{
public:
    void prepare (double newSampleRate, int maximumBlockSize, int channels)
    {
        sampleRate = std::max (1.0, newSampleRate);
        channelCount = std::max (1, channels);
        const auto requiredSamples = static_cast<int> (std::ceil (sampleRate * maxDelaySeconds))
                                   + maximumBlockSize + 8;

        buffers.assign (static_cast<size_t> (channelCount),
                        std::vector<float> (static_cast<size_t> (requiredSamples), 0.0f));
        writePosition = 0;
        lfoPhase = 0.0;

        rate.reset (sampleRate, smoothingSeconds);
        intensity.reset (sampleRate, smoothingSeconds);
        cents.reset (sampleRate, smoothingSeconds);
        delaySamples.reset (sampleRate, smoothingSeconds);
        rate.setCurrentAndTargetValue (5.0f);
        intensity.setCurrentAndTargetValue (0.5f);
        cents.setCurrentAndTargetValue (20.0f);
        delaySamples.setCurrentAndTargetValue (static_cast<float> (sampleRate * minimumDelaySeconds));
    }

    void reset()
    {
        for (auto& channel : buffers)
            std::fill (channel.begin(), channel.end(), 0.0f);
        writePosition = 0;
        lfoPhase = 0.0;
    }

    void setTargets (float rateHz, float intensityPercent, float peakCents)
    {
        rate.setTargetValue (juce::jlimit (0.0f, 10.0f, rateHz));
        intensity.setTargetValue (juce::jlimit (0.0f, 1.0f, intensityPercent * 0.01f));
        cents.setTargetValue (juce::jlimit (0.0f, 100.0f, peakCents));
    }

    void process (juce::AudioBuffer<float>& audio)
    {
        if (buffers.empty())
            return;

        const auto channelsToProcess = std::min (audio.getNumChannels(), channelCount);
        const auto bufferSize = static_cast<int> (buffers.front().size());

        for (int sample = 0; sample < audio.getNumSamples(); ++sample)
        {
            const auto currentRate = rate.getNextValue();
            const auto currentIntensity = intensity.getNextValue();
            const auto currentCents = cents.getNextValue();

            float targetDelay = static_cast<float> (sampleRate * minimumDelaySeconds);

            if (currentRate > 0.0001f && currentCents > 0.0001f)
            {
                const auto calibrationRate = std::max (currentRate, minimumCalibrationRateHz);
                const auto ratioDelta = std::pow (2.0, static_cast<double> (currentCents) / 1200.0) - 1.0;
                const auto excursion = ratioDelta * sampleRate
                                     / (juce::MathConstants<double>::twoPi * calibrationRate);
                const auto boundedExcursion = std::min (excursion,
                                                        sampleRate * (maxDelaySeconds * 0.45));
                const auto baseDelay = sampleRate * minimumDelaySeconds + boundedExcursion;
                targetDelay = static_cast<float> (baseDelay
                            + currentIntensity * boundedExcursion * std::cos (lfoPhase));
            }

            delaySamples.setTargetValue (targetDelay);
            const auto delay = juce::jlimit (1.0f,
                                             static_cast<float> (bufferSize - 4),
                                             delaySamples.getNextValue());

            for (int channel = 0; channel < channelsToProcess; ++channel)
            {
                auto* samples = audio.getWritePointer (channel);
                auto& ring = buffers[static_cast<size_t> (channel)];
                ring[static_cast<size_t> (writePosition)] = samples[sample];
                samples[sample] = readCubic (ring, static_cast<double> (writePosition) - delay);
            }

            writePosition = (writePosition + 1) % bufferSize;

            if (currentRate > 0.0001f)
            {
                lfoPhase += juce::MathConstants<double>::twoPi * currentRate / sampleRate;
                if (lfoPhase >= juce::MathConstants<double>::twoPi)
                    lfoPhase -= juce::MathConstants<double>::twoPi;
            }
        }
    }

private:
    static float readCubic (const std::vector<float>& ring, double position)
    {
        const auto size = static_cast<int> (ring.size());
        while (position < 0.0)
            position += size;
        while (position >= size)
            position -= size;

        const auto index = static_cast<int> (std::floor (position));
        const auto fraction = static_cast<float> (position - index);
        const auto at = [&ring, size] (int i)
        {
            i %= size;
            if (i < 0)
                i += size;
            return ring[static_cast<size_t> (i)];
        };

        const auto y0 = at (index - 1);
        const auto y1 = at (index);
        const auto y2 = at (index + 1);
        const auto y3 = at (index + 2);
        const auto a0 = -0.5f * y0 + 1.5f * y1 - 1.5f * y2 + 0.5f * y3;
        const auto a1 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
        const auto a2 = -0.5f * y0 + 0.5f * y2;
        return ((a0 * fraction + a1) * fraction + a2) * fraction + y1;
    }

    static constexpr double maxDelaySeconds = 0.5;
    static constexpr double minimumDelaySeconds = 0.001;
    static constexpr double smoothingSeconds = 0.025;
    static constexpr float minimumCalibrationRateHz = 0.05f;

    double sampleRate = 48000.0;
    int channelCount = 2;
    int writePosition = 0;
    double lfoPhase = 0.0;
    std::vector<std::vector<float>> buffers;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> rate;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> intensity;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> cents;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> delaySamples;
};
