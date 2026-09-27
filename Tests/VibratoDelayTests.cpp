#include "VibratoDelay.h"
#include <cmath>
#include <iostream>

int main()
{
    constexpr int blockSize = 256;
    constexpr double sampleRate = 48000.0;
    VibratoDelay effect;
    effect.prepare (sampleRate, blockSize, 2);
    effect.setTargets (5.0f, 50.0f, 20.0f);

    double phase = 0.0;
    double outputEnergy = 0.0;
    for (int block = 0; block < 120; ++block)
    {
        juce::AudioBuffer<float> audio (2, blockSize);
        for (int sample = 0; sample < blockSize; ++sample)
        {
            const auto input = static_cast<float> (0.25 * std::sin (phase));
            phase += juce::MathConstants<double>::twoPi * 220.0 / sampleRate;
            audio.setSample (0, sample, input);
            audio.setSample (1, sample, input);
        }

        effect.process (audio);
        for (int channel = 0; channel < audio.getNumChannels(); ++channel)
            for (int sample = 0; sample < audio.getNumSamples(); ++sample)
            {
                const auto value = audio.getSample (channel, sample);
                if (! std::isfinite (value))
                {
                    std::cerr << "Non-finite output\n";
                    return 1;
                }
                outputEnergy += static_cast<double> (value) * value;
            }

        for (int sample = 0; sample < blockSize; ++sample)
            if (std::abs (audio.getSample (0, sample) - audio.getSample (1, sample)) > 1.0e-6f)
            {
                std::cerr << "Stereo coherence failed\n";
                return 2;
            }
    }

    if (outputEnergy < 1.0)
    {
        std::cerr << "Output was unexpectedly silent\n";
        return 3;
    }

    std::cout << "VibratoDelay tests passed\n";
    return 0;
}
