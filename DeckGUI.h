/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 20 Feb 2025 3:14:45am
    Author:  Wei Keong

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "PlaylistComponent.h"
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "SpinningWheel.h"

class DeckGUI : public Component,
    public Button::Listener,
    public Slider::Listener,
    public FileDragAndDropTarget,
    public Timer
{
public:
    // Constructor takes a player reference and components needed for waveform display
    DeckGUI(DJAudioPlayer* player,
        AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& cacheToUse);

    // Destructor
    ~DeckGUI();

    // Component overrides for rendering and layout
    void paint(Graphics&) override;
    void resized() override;

    // Listener interface implementations
    void buttonClicked(Button*) override;  // Handles button click events
    void sliderValueChanged(Slider* slider) override;  // Handles slider value changes

    // FileDragAndDropTarget interface for drag and drop functionality
    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;

    // Timer callback for animations and updating UI components
    void timerCallback() override;

    // Public method to load an audio file from a URL
    void loadURL(const URL& audioURL);

private:
    // Helper method to set playback speed and update button appearance
    void setSpeedAndUpdateButtons(double speed, Button* activeButton);

    // File chooser for opening audio files
    juce::FileChooser fChooser{ "Select a file..." };

    // Playback control buttons
    TextButton playButton{ "PLAY" };
    TextButton stopButton{ "STOP" };
    TextButton pauseButton{ "PAUSE" };

    // Playback speed buttons
    TextButton speed025Button{ "2x" };
    TextButton speed050Button{ "1.5x" };
    TextButton speed100Button{ "1x" };
    TextButton speed150Button{ "0.5x" };
    TextButton speed175Button{ "0.25x" };

    // Cue point control buttons
    TextButton cueButton{ "Set" };
    TextButton jumpCueButton{ "Jump" };

    // Sliders for volume and position control
    Slider volSlider;  // Controls volume level
    Slider posSlider;  // Controls playback position

    // Visual components
    WaveformDisplay waveformDisplay;  // Displays audio waveform
    DJAudioPlayer* player;            // Reference to audio player
    SpinningWheel spinningWheel;      // Visual spinning wheel animation

    // State tracking variables
    bool isSongLoaded = false;     // Tracks if a song is currently loaded
    String trackName;              // Name of the currently loaded track
    int textOffset = 0;            // Offset for scrolling text animation
    bool isPlaying = false;        // Tracks if audio is currently playing

    // Prevents copying of this object
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI)
};