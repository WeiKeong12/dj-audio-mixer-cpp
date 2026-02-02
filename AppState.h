/*
  ==============================================================================

    AppState.h
    Created: 2 Mar 2025 5:36:54am
    Author:  Wei Keong

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <vector>

class AppState
{
public:
    // Constructor initializes the properties file system
    AppState();

    // Destructor ensures all pending changes are saved
    ~AppState();

    // Saves the current state of two audio tracks (paths and positions)
    void saveState(const String& track1, double position1,
        const String& track2, double position2);

    // Loads previously saved state for two audio tracks
    void loadState(String& track1, double& position1,
        String& track2, double& position2);

    // Saves a complete playlist including track titles, file paths, and durations
    void savePlaylist(const std::vector<juce::String>& trackTitles,
        const std::vector<juce::String>& trackPaths,
        const std::vector<juce::String>& trackDurations);

    // Loads a previously saved playlist into the provided vectors
    void loadPlaylist(std::vector<juce::String>& trackTitles,
        std::vector<juce::String>& trackPaths,
        std::vector<juce::String>& trackDurations);

private:
    // Internal properties file object that handles the actual file I/O
    std::unique_ptr<juce::PropertiesFile> propertiesFile;
};