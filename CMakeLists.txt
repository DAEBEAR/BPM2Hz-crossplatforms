cmake_minimum_required(VERSION 3.22)

# Nome del Progetto
project(BPM2Hz VERSION 1.0.0 LANGUAGES C CXX)

# Standard C++17 richiesto da JUCE 7/8
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------------------
# 1. RECUPERO O INCLUSIONE DI JUCE
# ------------------------------------------------------------------------------
# Se JUCE è stato scaricato come sottocartella o tramite FetchContent
if(NOT TARGET juce::juce_recommended_config_flags)
    add_subdirectory(_deps/juce-src EXCLUDE_FROM_ALL) # Modifica il percorso se la cartella JUCE è diversa
endif()

# ------------------------------------------------------------------------------
# 2. DEFINIZIONE DEL PLUGIN JUCE
# ------------------------------------------------------------------------------
juce_add_plugin(BPM2Hz
    COMPANY_NAME "DAEBAER"                      # Nome Produttore
    IS_SYNTH FALSE                              # FONDAMENTALE: Impostare su FALSE per permettere il Pass-Through Audio
    NEEDS_MIDI_INPUT FALSE                      # Impostare su TRUE solo se si inviano note MIDI al plugin
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE                # Copia automatica del VST3/AU nelle cartelle di sistema dopo la build
    PLUGIN_MANUFACTURER_CODE Daeb               # Codice ID Produttore (4 caratteri)
    PLUGIN_CODE B2hz                            # Codice ID Plugin (4 caratteri)
    FORMATS VST3 AU                             # Formati da compilare
    PRODUCT_NAME "BPM2Hz"
)

# ------------------------------------------------------------------------------
# 3. SORGENTI DEL PROGETTO
# ------------------------------------------------------------------------------
target_sources(BPM2Hz
    PRIVATE
        Source/PluginProcessor.h
        Source/PluginProcessor.cpp
        Source/PluginEditor.h
        Source/PluginEditor.cpp
)

# ------------------------------------------------------------------------------
# 4. GESTIONE DEGLI ASSET BINARI (Immagini, Logo, Sfondi)
# ------------------------------------------------------------------------------
# Compila e include automaticamente le risorse grafiche da BinaryData
juce_add_binary_data(AudioAssets
    SOURCES
        Source/Assets/brushed_metal.jpg
        Source/Assets/daebaer_logo.png
        Source/Assets/knob.png
)

# ------------------------------------------------------------------------------
# 5. LIBRERIE E MODULI JUCE COLLEGATI
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
# 6. COMPILAZIONE E DEFINIZIONI TARGET
# ------------------------------------------------------------------------------
target_compile_definitions(BPM2Hz
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)