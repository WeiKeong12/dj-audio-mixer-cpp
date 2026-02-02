/*
  ==============================================================================

    AppState.cpp
    Created: 2 Mar 2025 5:36:54am
    Author:  Wei Keong

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AppState.h"
#include "PlaylistComponent.h"

AppState::AppState()
{
    // Create options for the properties file
    juce::PropertiesFile::Options options;
    options.applicationName = "MyAudioApp";         // Name of the application
    options.filenameSuffix = ".settings";           // File extension for settings
    options.osxLibrarySubFolder = "Application Support"; // macOS specific folder location
    options.storageFormat = PropertiesFile::storeAsXML; // Store settings as XML format

    // Create the properties file with the configured options
    propertiesFile = std::make_unique<juce::PropertiesFile>(options);
}

// Destructor ensures that any pending changes are saved before the object is destroyed
AppState::~AppState()
{
    propertiesFile->save();
}

// Saves the current state of both audio tracks (paths and playback positions)
// to the properties file for persistence between sessions
void AppState::saveState(const String& track1, double position1,
    const String& track2, double position2)
{
    // Store the file paths for both tracks
    propertiesFile->setValue("track1", track1);
    propertiesFile->setValue("position1", position1);
    propertiesFile->setValue("track2", track2);
    propertiesFile->setValue("position2", position2);

    // Write changes to disk immediately
    propertiesFile->save();
}

// Loads the previously saved state of both audio tracks
// Default values are provided if the properties don't exist
void AppState::loadState(String& track1, double& position1,
    String& track2, double& position2)
{
    // Retrieve track file paths and positions, with defaults if not found
    track1 = propertiesFile->getValue("track1", "");            // Empty string as default
    position1 = propertiesFile->getDoubleValue("position1", 0.0); // 0.0 as default position
    track2 = propertiesFile->getValue("track2", "");
    position2 = propertiesFile->getDoubleValue("position2", 0.0);
}

// Saves the entire playlist data (titles, file paths, and durations)
// All data is stored as pipe-separated strings in the properties file
void AppState::savePlaylist(const std::vector<juce::String>& trackTitles,
    const std::vector<juce::String>& trackPaths,
    const std::vector<juce::String>& trackDurations)
{
    // Verify all vectors have the same size to ensure data integrity
    if (trackTitles.size() != trackPaths.size() || trackTitles.size() != trackDurations.size())
        return;

    // Clear existing playlist data before saving new data
    propertiesFile->removeValue("playlistTitles");
    propertiesFile->removeValue("playlistPaths");
    propertiesFile->removeValue("playlistDurations");

    // Prepare string arrays to hold the data
    juce::StringArray titleArray, pathArray, durationArray;

    // Populate the arrays with data from the vectors
    for (size_t i = 0; i < trackTitles.size(); ++i)
    {
        titleArray.add(trackTitles[i]);
        pathArray.add(trackPaths[i]);
        durationArray.add(trackDurations[i]);
    }

    // Join arrays into pipe-separated strings and save to properties file
    propertiesFile->setValue("playlistTitles", titleArray.joinIntoString("|"));
    propertiesFile->setValue("playlistPaths", pathArray.joinIntoString("|"));
    propertiesFile->setValue("playlistDurations", durationArray.joinIntoString("|"));

    // Write changes to disk
    propertiesFile->save();
}

// Loads the playlist data from the properties file into the provided vectors
void AppState::loadPlaylist(std::vector<juce::String>& trackTitles,
    std::vector<juce::String>& trackPaths,
    std::vector<juce::String>& trackDurations)
{
    // Clear existing vectors to prepare for loading
    trackTitles.clear();
    trackPaths.clear();
    trackDurations.clear();

    // Retrieve the stored strings from the properties file
    auto titleData = propertiesFile->getValue("playlistTitles", "");
    auto pathData = propertiesFile->getValue("playlistPaths", "");
    auto durationData = propertiesFile->getValue("playlistDurations", "");

    // Create string arrays to parse the pipe-separated values
    juce::StringArray titles, paths, durations;
    titles.addTokens(titleData, "|", "");       // Split by pipe character
    paths.addTokens(pathData, "|", "");
    durations.addTokens(durationData, "|", "");

    // Only populate vectors if all arrays have matching sizes (data integrity check)
    if (titles.size() == paths.size() && titles.size() == durations.size())
    {
        // Copy data from string arrays to the output vectors
        trackTitles.assign(titles.begin(), titles.end());
        trackPaths.assign(paths.begin(), paths.end());
        trackDurations.assign(durations.begin(), durations.end());
    }
}