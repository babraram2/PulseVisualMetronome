# Pulse Visual Metronome — JUCE VST3 project

This is the C++ source project for a visual metronome VST3 / Standalone app.

## Important
This package is the **buildable source project**, not a precompiled Windows `.vst3` binary.
A VST3 must be compiled with a Windows C++ toolchain and JUCE. This environment cannot
produce or verify a Windows VST3 binary, so no fake `.vst3` file is included.

## Features in this first implementation
- VST3 effect that passes audio through unchanged (no metronome audio is generated).
- Host tempo, PPQ, transport, time signature and elapsed-time reading where supplied by the DAW.
- Independent tempo mode, with BPM slider and tap tempo.
- 1–32 dot count.
- Individually assignable dot colors using a palette and HEX color entry.
- Bars/beats, bars/beats/ticks, elapsed seconds, minutes/seconds and beat-count display modes.
- Resizable dark interface and full-screen toggle.
- Saves dot colors, background, tempo, dot count and sync setting in plugin state.

## Build on Windows
1. Install Visual Studio 2022 with **Desktop development with C++** and the Windows 10/11 SDK.
2. Download JUCE 8.x from https://juce.com/get-juce/ and extract it, for example:
   `C:\\SDK\\JUCE`
3. Install CMake (3.22 or newer).
4. Open a Developer PowerShell for VS 2022.
5. From this project folder run:

   ```powershell
   cmake -S . -B build -DJUCE_DIR="C:/SDK/JUCE" -A x64
   cmake --build build --config Release
   ```

6. The VST3 should be produced under:
   `build/PulseVisualMetronome_artefacts/Release/VST3/Pulse Visual Metronome.vst3`
   (exact intermediate folder layout can vary slightly by JUCE/CMake version).
7. Copy the `.vst3` bundle into:
   `%COMMONPROGRAMFILES%\\VST3`
8. Rescan plugins in Bitwig Studio.

## Notes / current limitations
- Independent mode advances while the host calls the plugin's audio process callback. In a DAW this normally means the track must be active and transport running. A future standalone-specific clock can run independently of a DAW transport.
- The current UI is a functional first build, not yet the full advanced design: custom palette, HEX color, dot count, host sync, tap tempo, time display and full-screen are included. Per-dot fade styles, swing/subdivision visualization, presets, separate remaining-duration display, and a dedicated detached second-window display remain planned.
- Host timing quality depends on the position information supplied by the DAW. The plugin does not generate audio clicks.
