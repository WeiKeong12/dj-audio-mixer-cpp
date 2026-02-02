/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once 
#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"
#include "CrossFaderAndIsolation.h"
#include "AppState.h"

class MainComponent : public AudioAppComponent
{
public:
    // Constructor & Destructor
    MainComponent();
    ~MainComponent();

    // AudioAppComponent overrides
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // GUI Rendering
    void paint(Graphics& g) override;
    void resized() override;

private:
    //==============================================================================

    // Audio format manager (handles different file formats like MP3, WAV)
    AudioFormatManager formatManager;

    // Cache for waveform thumbnails (stores up to 100 thumbnails)
    AudioThumbnailCache thumbCache{ 100 };

    // First audio player and its GUI
    DJAudioPlayer player1{ formatManager };
    DeckGUI deckGUI1{ &player1, formatManager, thumbCache };

    // Second audio player and its GUI
    DJAudioPlayer player2{ formatManager };
    DeckGUI deckGUI2{ &player2, formatManager, thumbCache };

    // Mixer to blend the audio sources from both players
    MixerAudioSource mixerSource;

    // Stores application state (e.g., track positions, last played tracks)
    AppState appState;

    // Save and load application state
    void saveApplicationState();
    void loadApplicationState();

    // Playlist component to manage track selection
    PlaylistComponent playlistComponent{ &player1, &player2 };

    // Crossfader and equalizer controls for blending tracks
    CrossFaderAndIsolation crossFaderAndIsolation{ &player1, &player2 };

    // Spinning wheel visual for deck 1
    SpinningWheel spinningWheel{ &player1 };

    // Prevents copying or memory leaks
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};