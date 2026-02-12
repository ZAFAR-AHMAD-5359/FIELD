cmake_minimum_required(VERSION 3.22)

project(FIELD VERSION 2.0.0)

# C++17 required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch JUCE 7 (stable version for reliable CI/CD builds)
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# Plugin formats
if(APPLE)
    set(FORMATS AU VST3)
elseif(WIN32)
    set(FORMATS VST3)
else()
    set(FORMATS VST3)
endif()

# Add plugin target
juce_add_plugin(FIELD
    COMPANY_NAME "Ragein Audio"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Rage
    PLUGIN_CODE Fiel
    FORMATS ${FORMATS}
    PRODUCT_NAME "FIELD"
    BUNDLE_ID "com.rageinaudio.field"
    COMPANY_WEBSITE "https://rageinaudio.com"
    COMPANY_EMAIL "contact@rageinaudio.com"
    JUCE_GENERATE_JUCE_HEADER ON
)

# Source files
target_sources(FIELD
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginEditor.cpp
        src/DelayLine.cpp
        src/BiquadFilter.cpp
)

# Compile definitions
target_compile_definitions(FIELD
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Link libraries
target_link_libraries(FIELD
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# macOS specific settings
if(APPLE)
    set_target_properties(FIELD PROPERTIES
        XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "11.0"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ""
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
    )
endif()
