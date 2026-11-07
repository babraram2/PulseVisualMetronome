#pragma once
#include <JuceHeader.h>
#include <array>

class PulseVisualMetronomeAudioProcessor : public juce::AudioProcessor
{
public:
    PulseVisualMetronomeAudioProcessor();
    ~PulseVisualMetronomeAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double get tailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    bool isSynchronized() const { return syncToHost.load(); }
    void setSynchronized(bool b) { syncToHost.store(b); }
    double getBpm() const { return bpm.load(); }
    void setBpm(double v) { bpm.store(juce::jlimit(20.0, 400.0, v)); }
    int getDotCount() const { return dotCount.load(); }
    void setDotCount(int n) { dotCount.store(juce::jlimit(1, 32, n)); }
    int getActiveDot() const { return activeDot.load(); }
    double getPositionSeconds() const { return positionSeconds.load(); }
    double getPositionPpq() const { return positionPpq.load(); }
    int getNumerator() const { return numerator.load(); }
    int getDenominator() const { return denominator.load(); }
    bool isHostPlaying() const { return hostPlaying.load(); }

    juce::Colour getDotColour(int i) const;
    void setDotColour(int i, juce::Colour c);
    juce::Colour getBackgroundColour() const { return backgroundColour; }
    void setBackgroundColour(juce::Colour c) { backgroundColour = c; }

private:
    std::atomic<bool> syncToHost { true };
    std::atomic<double> bpm { 120.0 };
    std::atomic<int> dotCount { 4 };
    std::atomic<int> activeDot { -1 };
    std::atomic<double> positionSeconds { 0.0 }, positionPpq { 0.0 };
    std::atomic<int> numerator { 4 }, denominator { 4 };
    std::atomic<bool> hostPlaying { false };
    double sampleRateHz = 44100.0;
    double independentSeconds = 0.0;
    std::array<juce::Colour, 32> dotColours;
    juce::Colour backgroundColour { 0xff111722 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PulseVisualMetronomeAudioProcessor)
};
