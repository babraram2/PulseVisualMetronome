cmake_minimum_required(VERSION 3.22)
project(PulseVisualMetronome VERSION 1.0.0)

# Set JUCE_DIR to a local JUCE checkout, or place JUCE beside this project.
if(NOT DEFINED JUCE_DIR)
    set(JUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE" CACHE PATH "Path to JUCE")
endif()
add_subdirectory("${JUCE_DIR}" JUCE)

juce_add_plugin(PulseVisualMetronome
    COMPANY_NAME "Pulse Audio"
    BUNDLE_ID "com.pulseaudio.pulsevisualmetronome"
    PLUGIN_MANUFACTURER_CODE PlsA
    PLUGIN_CODE PvM1
    FORMATS VST3 Standalone
    PRODUCT_NAME "Pulse Visual Metronome"
    DESCRIPTION "Customizable visual metronome"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(PulseVisualMetronome PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)
target_compile_features(PulseVisualMetronome PRIVATE cxx_std_17)
target_compile_definitions(PulseVisualMetronome PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)
target_link_libraries(PulseVisualMetronome PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_gui_extra
    PUBLIC juce::juce_recommended_config_flags
           juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags
)

juce_generate_juce_header(PulseVisualMetronome)