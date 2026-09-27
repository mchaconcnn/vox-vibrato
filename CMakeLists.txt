cmake_minimum_required(VERSION 3.24)

set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum macOS version" FORCE)

project(VoxVibrato VERSION 0.1.2 LANGUAGES C CXX OBJC OBJCXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 9.0.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(VoxVibrato
    COMPANY_NAME "rco.cr"
    BUNDLE_ID "com.marcochaconmora.voxvibrato"
    PLUGIN_MANUFACTURER_CODE Mcmr
    PLUGIN_CODE Vxvb
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "Vox Vibrato"
    DESCRIPTION "Musical pitch vibrato for vocals"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(VoxVibrato PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/VibratoDelay.h
)

target_compile_definitions(VoxVibrato PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(VoxVibrato
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

juce_generate_juce_header(VoxVibrato)

add_executable(VoxVibratoTests Tests/VibratoDelayTests.cpp)
target_include_directories(VoxVibratoTests PRIVATE Source)
target_link_libraries(VoxVibratoTests PRIVATE
    juce::juce_audio_basics
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
)

enable_testing()
add_test(NAME VibratoDelayTests COMMAND VoxVibratoTests)
