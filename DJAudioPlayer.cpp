/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 19 Feb 2025 2:10:15pm
    Author:  Wei Keong

  ==============================================================================
*/

#include "DJAudioPlayer.h"
#include "DeckGUI.h"

// Constructor: Initializes the DJAudioPlayer with the provided format manager
DJAudioPlayer::DJAudioPlayer(AudioFormatManager& _formatManager)
    : formatManager(_formatManager)
{

}

// Destructor: Cleans up resources when the object is destroyed
DJAudioPlayer::~DJAudioPlayer()
{

}

// Retrieves the next block of audio data for playback
void DJAudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // If no audio source is loaded, clear the buffer and return
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    // Fetch audio from the resampling source
    resampleSource.getNextAudioBlock(bufferToFill);

    // Remove vocals if enabled (stereo channels only)
    if (removeVocals && bufferToFill.buffer->getNumChannels() >= 2)
    {
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            float left = bufferToFill.buffer->getSample(0, sample);
            float right = bufferToFill.buffer->getSample(1, sample);

            // Vocal removal by phase cancellation
            float instrumental = (left - right) * 0.5f;
            bufferToFill.buffer->setSample(0, sample, instrumental);
            bufferToFill.buffer->setSample(1, sample, instrumental);
        }
    }
}

// Prepares the player for audio playback
void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

// Releases allocated resources when playback stops
void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

// Loads an audio file from a given URL
void DJAudioPlayer::loadURL(URL audioURL)
{
    currentURL = audioURL;

    // Create a reader for the audio file
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr)
    {
        // Create a new audio source
        std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));

        // Reset transport source and resample source
        transportSource.setSource(nullptr);
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        resampleSource.setResamplingRatio(1.0);

        // Store the new audio source
        readerSource.reset(newSource.release());
    }
}

// Sets the gain (volume) of the audio player
void DJAudioPlayer::setGain(double gain)
{
    // Ensure gain is within a valid range (0 to 1)
    if (gain < 0 || gain > 1.0)
    {
        std::cout << "DJAudioPlayer::setGain gain should be between 0 and 1" << std::endl;
    }
    else
    {
        transportSource.setGain(gain);
    }
}

// Sets the playback speed by adjusting the resampling ratio
void DJAudioPlayer::setSpeed(double ratio)
{
    // Ensure speed is within a valid range (0.1x to 3.0x)
    if (ratio < 0.1 || ratio > 3.0)
    {
        std::cout << "DJAudioPlayer::setSpeed ratio should be between 0.1 and 3.0" << std::endl;
    }
    else
    {
        double resamplingRatio = 1.0 / ratio;
        resampleSource.setResamplingRatio(resamplingRatio);
        std::cout << "Speed set to: " << ratio << "x (resampling ratio: " << resamplingRatio << ")" << std::endl;
    }
}

// Sets the playback position in seconds
void DJAudioPlayer::setPosition(double posInSecs)
{
    transportSource.setPosition(posInSecs);
}

// Sets the playback position as a percentage of the track length
void DJAudioPlayer::setPositionRelative(double pos)
{
    // Ensure position is within a valid range (0 to 1)
    if (pos < 0 || pos > 1.0)
    {
        std::cout << "DJAudioPlayer::setPositionRelative pos should be between 0 and 1" << std::endl;
    }
    else
    {
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
}

// Starts playback
void DJAudioPlayer::start()
{
    transportSource.start();
}

// Stops playback
void DJAudioPlayer::stop()
{
    transportSource.stop();
}

// Pauses playback (same as stop for now)
void DJAudioPlayer::pause()
{
    transportSource.stop();
}

// Gets the current playback position as a percentage of the track length
double DJAudioPlayer::getPositionRelative()
{
    return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
}

// Returns the total length of the loaded track in seconds
double DJAudioPlayer::getTrackLength()
{
    if (transportSource.getLengthInSeconds() > 0)
        return transportSource.getLengthInSeconds();
    return 0.0;
}

// Returns the current playback position in seconds
double DJAudioPlayer::getPosition()
{
    return transportSource.getCurrentPosition();
}

// Sets a cue point at the current playback position
void DJAudioPlayer::setCuePoint()
{
    cuePoint = getPosition();
    std::cout << "Cue set at: " << cuePoint << " seconds" << std::endl;
}

// Returns the stored cue point position
double DJAudioPlayer::getCuePoint()
{
    return cuePoint;
}

// Jumps to the stored cue point and starts playback
void DJAudioPlayer::jumpToCue()
{
    if (cuePoint > 0.0)
    {
        setPosition(cuePoint);
        start();
    }
}