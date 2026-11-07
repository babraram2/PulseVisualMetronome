#include "PluginEditor.h"

PulseVisualMetronomeAudioProcessorEditor::PulseVisualMetronomeAudioProcessorEditor(
    PulseVisualMetronomeAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(760, 590);
    setResizable(true, true);
    setResizeLimits(560, 430, 1500, 1100);

    dotCountBox.addItemList({ "1", "2", "3", "4", "5", "6", "7", "8",
                              "9", "10", "12", "16", "24", "32" }, 1);
    dotCountBox.setSelectedId(4);
    dotCountBox.onChange = [this]
    {
        const int values[] = {1,2,3,4,5,6,7,8,9,10,12,16,24,32};
        processor.setDotCount(values[dotCountBox.getSelectedItemIndex()]);
        repaint();
    };
    addAndMakeVisible(dotCountBox);

    syncButton.setToggleState(true, juce::dontSendNotification);
    syncButton.onClick = [this] { processor.setSynchronized(syncButton.getToggleState()); };
    addAndMakeVisible(syncButton);

    bpmSlider.setRange(20.0, 400.0, 0.01);
    bpmSlider.setValue(120.0);
    bpmSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 24);
    bpmSlider.onValueChange = [this] { if (!processor.isSynchronized()) processor.setBpm(bpmSlider.getValue()); };
    addAndMakeVisible(bpmSlider);

    timeModeBox.addItem("Bars : Beats", 1);
    timeModeBox.addItem("Bars : Beats : Ticks", 2);
    timeModeBox.addItem("Elapsed seconds", 3);
    timeModeBox.addItem("Minutes : Seconds", 4);
    timeModeBox.addItem("Beat count", 5);
    timeModeBox.setSelectedId(1);
    addAndMakeVisible(timeModeBox);

    for (int i = 0; i < 32; ++i)
    {
        dotButtons[i].setButtonText(juce::String(i + 1));
        dotButtons[i].onClick = [this, i] { selectedDot = i; repaint(); };
        addAndMakeVisible(dotButtons[i]);
    }

    const juce::Colour palette[] = {
        juce::Colour(0xffffb52e), juce::Colour(0xffff5252), juce::Colour(0xffff8a65),
        juce::Colour(0xffe040fb), juce::Colour(0xffab47bc), juce::Colour(0xff7e57c2),
        juce::Colour(0xff536dfe), juce::Colour(0xff448aff), juce::Colour(0xff26c6da),
        juce::Colour(0xff26a69a), juce::Colour(0xff66bb6a), juce::Colour(0xffd4e157),
        juce::Colours::white, juce::Colour(0xffb0bec5), juce::Colour(0xff78909c),
        juce::Colour(0xff455a64), juce::Colour(0xff303030), juce::Colours::black
    };
    for (int i = 0; i < 18; ++i)
    {
        paletteButtons[i].setButtonText("");
        paletteButtons[i].setColour(juce::TextButton::buttonColourId, palette[i]);
        paletteButtons[i].onClick = [this, i]
        {
            const juce::Colour colours[] = {
                juce::Colour(0xffffb52e), juce::Colour(0xffff5252), juce::Colour(0xffff8a65),
                juce::Colour(0xffe040fb), juce::Colour(0xffab47bc), juce::Colour(0xff7e57c2),
                juce::Colour(0xff536dfe), juce::Colour(0xff448aff), juce::Colour(0xff26c6da),
                juce::Colour(0xff26a69a), juce::Colour(0xff66bb6a), juce::Colour(0xffd4e157),
                juce::Colours::white, juce::Colour(0xffb0bec5), juce::Colour(0xff78909c),
                juce::Colour(0xff455a64), juce::Colour(0xff303030), juce::Colours::black
            };
            processor.setDotColour(selectedDot, colours[i]);
            repaint();
        };
        addAndMakeVisible(paletteButtons[i]);
    }

    hexEditor.setText("#FFB52E");
    hexEditor.setInputRestrictions(7, "0123456789ABCDEFabcdef#");
    hexEditor.setJustification(juce::Justification::centred);
    addAndMakeVisible(hexEditor);
    applyHexButton.onClick = [this]
    {
        auto s = hexEditor.getText().trim().removeCharacters("#");
        if (s.length() == 6)
            processor.setDotColour(selectedDot, juce::Colour::fromString("FF" + s));
        repaint();
    };
    addAndMakeVisible(applyHexButton);

    tapButton.onClick = [this]
    {
        static double last = 0.0;
        const double now = juce::Time::getMillisecondCounterHiRes();
        if (last > 0.0)
        {
            const double interval = now - last;
            if (interval > 150.0 && interval < 3000.0)
            {
                processor.setBpm(60000.0 / interval);
                bpmSlider.setValue(processor.getBpm(), juce::sendNotificationSync);
            }
        }
        last = now;
    };
    addAndMakeVisible(tapButton);
    resetButton.onClick = [this] { bpmSlider.setValue(120.0); processor.setBpm(120.0); };
    addAndMakeVisible(resetButton);
    fullScreenButton.onClick = [this]
    {
        if (auto* w = dynamic_cast<juce::ResizableWindow*>(getTopLevelComponent()))
        {
            fullScreen = !fullScreen;
            w->setFullScreen(fullScreen);
        }
    };
    addAndMakeVisible(fullScreenButton);

    bpmLabel.setText("TEMPO / BPM", juce::dontSendNotification);
    positionLabel.setText("BAR : BEAT : TICK", juce::dontSendNotification);
    timeLabel.setText("POSITION", juce::dontSendNotification);
    dotLabel.setText("DOT COUNT", juce::dontSendNotification);
    paletteLabel.setText("INDIVIDUAL DOT COLOR  •  SELECT A DOT, THEN A COLOR", juce::dontSendNotification);
    for (auto* l : { &bpmLabel, &positionLabel, &timeLabel, &dotLabel, &paletteLabel })
    {
        l->setColour(juce::Label::textColourId, juce::Colour(0xffaab6c8));
        addAndMakeVisible(*l);
    }
    startTimerHz(30);
}

void PulseVisualMetronomeAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff111722));
    g.setColour(juce::Colour(0xff1b2533));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(12.0f), 16.0f);

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(26.0f, juce::Font::bold));
    g.drawText("PULSE", 30, 18, 180, 38, juce::Justification::left);
    g.setColour(juce::Colour(0xff8290a5));
    g.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    g.drawText("VISUAL METRONOME  •  VST3", 32, 52, 240, 18, juce::Justification::left);

    const int count = processor.getDotCount();
    const auto area = juce::Rectangle<float>(35.0f, 120.0f, (float)getWidth() - 70.0f, 112.0f);
    const float diameter = juce::jlimit(18.0f, 72.0f,
        juce::jmin(72.0f, (area.getWidth() - (count - 1) * 12.0f) / (float)count));
    const float gap = count > 1 ? (area.getWidth() - count * diameter) / (float)(count - 1) : 0.0f;
    const float total = count * diameter + (count - 1) * gap;
    float x = (getWidth() - total) * 0.5f;
    const int active = processor.getActiveDot();

    for (int i = 0; i < count; ++i)
    {
        auto colour = processor.getDotColour(i);
        const bool on = (i == active);
        const float d = on ? diameter : diameter * 0.78f;
        const float y = area.getCentreY() - d * 0.5f;
        if (on)
        {
            g.setColour(colour.withAlpha(0.22f));
            g.fillEllipse(x - 10.0f, y - 10.0f, d + 20.0f, d + 20.0f);
        }
        g.setColour(on ? colour : colour.withAlpha(0.22f));
        g.fillEllipse(x + (diameter - d) * 0.5f, y, d, d);
        g.setColour(juce::Colour(0xff455267));
        g.drawEllipse(x + (diameter - d) * 0.5f, y, d, d, 1.0f);
        x += diameter + gap;
    }

    g.setColour(juce::Colour(0xffaab6c8));
    g.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    g.drawText("BEAT LIGHTS", 35, 92, 180, 18, juce::Justification::left);
    g.drawText(processor.isSynchronized() ? "HOST SYNC" : "INDEPENDENT CLOCK",
               getWidth() - 220, 92, 185, 18, juce::Justification::right);

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(25.0f, juce::Font::bold));
    g.drawText(juce::String(processor.getBpm(), 2) + " BPM",
               35, 250, 230, 38, juce::Justification::left);
    g.setColour(juce::Colour(0xffaab6c8));
    g.setFont(juce::FontOptions(12.0f));
    g.drawText("POSITION", 35, 300, 100, 18, juce::Justification::left);
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(23.0f, juce::Font::bold));
    g.drawText(positionLabel.getText(), 35, 320, 300, 34, juce::Justification::left);
    g.setColour(juce::Colour(0xffaab6c8));
    g.setFont(juce::FontOptions(12.0f));
    g.drawText(timeLabel.getText(), 370, 300, 180, 18, juce::Justification::left);
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(23.0f, juce::Font::bold));
    g.drawText(juce::String(processor.getPositionSeconds(), 2) + " s",
               370, 320, 300, 34, juce::Justification::left);
}

void PulseVisualMetronomeAudioProcessorEditor::resized()
{
    auto w = getWidth();
    syncButton.setBounds(w - 150, 26, 115, 28);
    dotLabel.setBounds(35, 375, 120, 20);
    dotCountBox.setBounds(155, 373, 90, 26);
    bpmLabel.setBounds(270, 375, 110, 20);
    bpmSlider.setBounds(385, 366, 180, 48);
    tapButton.setBounds(575, 373, 55, 26);
    resetButton.setBounds(635, 373, 75, 26);
    timeModeBox.setBounds(35, 420, 180, 28);
    fullScreenButton.setBounds(w - 150, 420, 115, 28);

    const int count = processor.getDotCount();
    const int visible = juce::jmin(count, 32);
    const int cols = juce::jmin(16, visible);
    const int cellW = juce::jmax(28, (w - 70) / juce::jmax(1, cols));
    for (int i = 0; i < 32; ++i)
    {
        if (i < visible)
        {
            const int row = i / cols;
            const int col = i % cols;
            dotButtons[i].setBounds(35 + col * cellW, 465 + row * 28, cellW - 5, 23);
            dotButtons[i].setVisible(true);
            dotButtons[i].setColour(juce::TextButton::buttonColourId,
                i == selectedDot ? juce::Colour(0xff40536f) : juce::Colour(0xff263346));
        }
        else dotButtons[i].setVisible(false);
    }

    paletteLabel.setBounds(35, 520, w - 70, 18);
    const int pW = juce::jmax(22, (w - 70) / 18);
    for (int i = 0; i < 18; ++i)
        paletteButtons[i].setBounds(35 + i * pW, 542, pW - 4, 20);
    hexEditor.setBounds(35, 566, 100, 22);
    applyHexButton.setBounds(142, 566, 80, 22);
}

void PulseVisualMetronomeAudioProcessorEditor::timerCallback()
{
    updateReadout();
    repaint();
}

void PulseVisualMetronomeAudioProcessorEditor::updateReadout()
{
    const double ppq = processor.getPositionPpq();
    const int num = processor.getNumerator();
    const int den = processor.getDenominator();
    const double beatsPerBar = num * 4.0 / den;
    const int bar = (int)std::floor(ppq / juce::jmax(0.001, beatsPerBar)) + 1;
    const double beatInBar = std::fmod(std::max(0.0, ppq), beatsPerBar);
    const int beat = (int)std::floor(beatInBar * den / 4.0) + 1;
    const int ticks = (int)std::round((beatInBar * den / 4.0 - std::floor(beatInBar * den / 4.0)) * 960.0);
    switch (timeModeBox.getSelectedId())
    {
        case 1: positionLabel.setText(juce::String(bar).paddedLeft('0', 3) + ":" +
                                      juce::String(beat).paddedLeft('0', 2), juce::dontSendNotification); break;
        case 2: positionLabel.setText(juce::String(bar).paddedLeft('0', 3) + ":" +
                                      juce::String(beat).paddedLeft('0', 2) + ":" +
                                      juce::String(ticks).paddedLeft('0', 3), juce::dontSendNotification); break;
        case 3: positionLabel.setText(juce::String(processor.getPositionSeconds(), 3) + " s", juce::dontSendNotification); break;
        case 4:
        {
            const int sec = (int)processor.getPositionSeconds();
            positionLabel.setText(juce::String(sec / 60).paddedLeft('0', 2) + ":" +
                                  juce::String(sec % 60).paddedLeft('0', 2), juce::dontSendNotification);
            break;
        }
        case 5: positionLabel.setText(juce::String((int)std::floor(ppq * den / 4.0) + 1), juce::dontSendNotification); break;
        default: break;
    }
}

void PulseVisualMetronomeAudioProcessorEditor::chooseColourForSelectedDot() {}
void PulseVisualMetronomeAudioProcessorEditor::addPaletteButton(juce::Colour) {}
