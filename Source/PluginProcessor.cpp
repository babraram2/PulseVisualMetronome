#include "PluginProcessor.h"
#include "PluginEditor.h"

PulseVisualMetronomeAudioProcessor::PulseVisualMetronomeAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    const juce::Colour defaults[] = {
        juce::Colour(0xffffb52e), juce::Colour(0xff4b91ff),
        juce::Colour(0xff35c99a), juce::Colour(0xffe45c78)
    };
    for (int i = 0; i < 32; ++i)
        dotColours[(size_t)i] = defaults[i % 4];
}

void PulseVisualMetronomeAudioProcessor::prepareToPlay(double sr, int)
{
    sampleRateHz = sr > 0.0 ? sr : 44100.0;
    independentSeconds = 0.0;
}

void PulseVisualMetronomeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                       juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    // Transparent audio-through effect: the plugin does not add sound.
    juce::AudioPlayHead::PositionInfo info;
    bool gotPosition = false;
    if (auto* ph = getPlayHead())
        if (auto p = ph->getPosition())
        {
            info = *p;
            gotPosition = true;
        }

    if (syncToHost.load() && gotPosition)
    {
        const auto hostBpm = info.getBpm();
        if (hostBpm.hasValue())
            bpm.store(juce::jlimit(20.0, 400.0, *hostBpm));

        const auto ts = info.getTimeSignature();
        if (ts.hasValue())
        {
            numerator.store(juce::jlimit(1, 32, ts->numerator));
            denominator.store(juce::jlimit(1, 32, ts->denominator));
        }

        hostPlaying.store(info.getIsPlaying());
        const auto ppq = info.getPpqPosition();
        if (ppq.hasValue())
            positionPpq.store(*ppq);
        const auto secs = info.getTimeInSeconds();
        if (secs.hasValue())
            positionSeconds.store(*secs);

        const double beatsPerBar = (double) numerator.load() * 4.0 / (double) denominator.load();
        const double ppqNow = positionPpq.load();
        const double barBeat = std::fmod(std::max(0.0, ppqNow), std::max(0.0001, beatsPerBar));
        const int beatIndex = (int) std::floor(barBeat * denominator.load() / 4.0 + 1.0e-6);
        activeDot.store(hostPlaying.load() ? (beatIndex % dotCount.load()) : -1);
    }
    else
    {
        independentSeconds += (double) buffer.getNumSamples() / sampleRateHz;
        positionSeconds.store(independentSeconds);
        const double beatLength = 60.0 / bpm.load();
        const auto totalBeats = (long long) std::floor(independentSeconds / beatLength);
        positionPpq.store((double) totalBeats);
        numerator.store(dotCount.load());
        denominator.store(4);
        activeDot.store((int)(totalBeats % dotCount.load()));
        hostPlaying.store(true);
    }
}

juce::Colour PulseVisualMetronomeAudioProcessor::getDotColour(int i) const
{
    return dotColours[(size_t)juce::jlimit(0, 31, i)];
}

void PulseVisualMetronomeAudioProcessor::setDotColour(int i, juce::Colour c)
{
    dotColours[(size_t)juce::jlimit(0, 31, i)] = c;
}

juce::AudioProcessorEditor* PulseVisualMetronomeAudioProcessor::createEditor()
{
    return new PulseVisualMetronomeAudioProcessorEditor(*this);
}

void PulseVisualMetronomeAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    juce::ValueTree state("PULSE_STATE");
    state.setProperty("sync", syncToHost.load(), nullptr);
    state.setProperty("bpm", bpm.load(), nullptr);
    state.setProperty("dots", dotCount.load(), nullptr);
    state.setProperty("background", (juce::int64)backgroundColour.getARGB(), nullptr);
    for (int i = 0; i < 32; ++i)
        state.setProperty("dot" + juce::String(i), (juce::int64)getDotColour(i).getARGB(), nullptr);
    if (auto xml = state.createXml())
        copyXmlToBinary(*xml, dest);
}

void PulseVisualMetronomeAudioProcessor::setStateInformation(const void* data, int size)
{
    if (auto xml = getXmlFromBinary(data, size))
    {
        auto state = juce::ValueTree::fromXml(*xml);
        if (!state.isValid()) return;
        syncToHost.store((bool)state.getProperty("sync", true));
        bpm.store((double)state.getProperty("bpm", 120.0));
        dotCount.store((int)state.getProperty("dots", 4));
        backgroundColour = juce::Colour((juce::uint32)(juce::int64)state.getProperty("background", (juce::int64)0xff111722));
        for (int i = 0; i < 32; ++i)
        {
            const auto key = "dot" + juce::String(i);
            if (state.hasProperty(key))
                dotColours[(size_t)i] = juce::Colour((juce::uint32)(juce::int64)state.getProperty(key));
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PulseVisualMetronomeAudioProcessor();
}
