cmake_minimum_required(VERSION 3.22)
project(BPM2Hz VERSION 1.0.0)

set(JUCE_BUILD_HELPER_TOOLS OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(BPM2Hz
    COMPANY_NAME "DAEBAER Plugins"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE DAEB
    PLUGIN_CODE B2Hz
    JUCE_VST3_CAN_REPLACE_VST2 FALSE
    FORMATS VST3 AU
    PRODUCT_NAME "BPM2Hz"
)

juce_generate_juce_header(BPM2Hz)

juce_add_binary_data(AudioAssets
    SOURCES
        assets/knob.png
        assets/brushed_metal.jpg
        assets/daebaer_logo.png
)

target_sources(BPM2Hz
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
)

target_compile_definitions(BPM2Hz
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(BPM2Hz
    PRIVATE
        AudioAssets
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)