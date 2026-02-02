/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 19 Feb 2025 2:10:15pm
    Author:  Wei Keong

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "AppState.h"

class DJAudioPlayer : public AudioSource
{
public:

    DJAudioPlayer(AudioFormatManager& _formatManager);
    ~DJAudioPlayer();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void loadURL(URL audioURL);

    void setGain(double gain);
    void setSpeed(double ratio);
    void setPosition(double posInSecs);

    // Sets the playback position as a percentage of the track length (0 to 1 range)
    void setPositionRelative(double pos);
    void start();
    void stop();
    void pause();
    void setCuePoint();
    void jumpToCue();

    // Gets the current playback position as a percentage of track length
    double getPositionRelative();
    double getTrackLength();
    double getPosition();
    double getCuePoint();

    // Returns the current audio file URL
    URL getCurrentURL() const { return currentURL; }
    // Returns a reference to the resampling source (for external control)
    ResamplingAudioSource& getResampleSource() { return resampleSource; }

private:
    // Reference to the AudioFormatManager for handling audio formats
    AudioFormatManager& formatManager;

    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    ResamplingAudioSource resampleSource{ &transportSource, false, 2 };
    URL currentURL;
    // Stores the cue point position in seconds
    double cuePoint = 0.0;
    // Flag for enabling/disabling vocal removal
    bool removeVocals = false;
};
