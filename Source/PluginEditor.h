#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class PulseVisualMetronomeAudioProcessorEditor : public juce::AudioProcessorEditor,
                                                  private juce::Timer
{
public:
    explicit PulseVisualMetronomeAudioProcessorEditor(PulseVisualMetronomeAudioProcessor&);
    ~PulseVisualMetronomeAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateReadout();
    void chooseColourForSelectedDot();
    void addPaletteButton(juce::Colour);

    PulseVisualMetronomeAudioProcessor& processor;
    juce::ComboBox dotCountBox, timeModeBox;
    juce::ToggleButton syncButton { "DAW SYNC" };
    juce::Slider bpmSlider;
    juce::Label bpmLabel, positionLabel, timeLabel, dotLabel, paletteLabel;
    juce::TextButton tapButton { "TAP" }, resetButton { "RESET" }, fullScreenButton { "FULL SCREEN" };
    juce::TextButton dotButtons[32];
    juce::TextButton paletteButtons[18];
    juce::TextEditor hexEditor;
    juce::TextButton applyHexButton { "SET HEX" };
    int selectedDot = 0;
    int blinkPhase = 0;
    bool fullScreen = false;
    juce::Rectangle<int> originalBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PulseVisualMetronomeAudioProcessorEditor)
};
