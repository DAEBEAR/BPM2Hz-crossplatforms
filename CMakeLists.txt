cmake_minimum_required(VERSION 3.22)

project(BPM2Hz VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------------------
# 1. FETCHCONTENT DI JUCE
# ------------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.0
)

FetchContent_MakeAvailable(juce)

# ------------------------------------------------------------------------------
# 2. DEFINIZIONE DEL PLUGIN JUCE
# ------------------------------------------------------------------------------
juce_add_plugin(BPM2Hz
    COMPANY_NAME "DAEBAER"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Daeb
    PLUGIN_CODE B2hz
    FORMATS VST3 AU
    PRODUCT_NAME "BPM2Hz"
)

# ------------------------------------------------------------------------------
# 3. SORGENTI
# ------------------------------------------------------------------------------
target_sources(BPM2Hz
    PRIVATE
        Source/PluginProcessor.h
        Source/PluginProcessor.cpp
        Source/PluginEditor.h
        Source/PluginEditor.cpp
)

# ------------------------------------------------------------------------------
# 4. GESTIONE ASSET BINARI
# ------------------------------------------------------------------------------
juce_add_binary_data(AudioAssets
    SOURCES
        Source/brushed_metal.jpg
        Source/daebaer_logo.png
)

# ------------------------------------------------------------------------------
# 5. LIBRERIE LINKATE
# ------------------------------------------------------------------------------
target_link_libraries(BPM2Hz
    PRIVATE
        AudioAssets
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_extra
        juce::juce_gui_basics
        juce::juce_graphics
        juce::juce_events
        juce::juce_core
        juce::juce_data_structures
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# ------------------------------------------------------------------------------
# 6. DEFINIZIONI TARGET
# ------------------------------------------------------------------------------
target_compile_definitions(BPM2Hz
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)