/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "DJAudioPlayer.h"

//==============================================================================
MainComponent::MainComponent()
    : crossFaderAndIsolation(&player1, &player2),  // Crossfader to blend two decks
    spinningWheel(&player1),  // Visualization for deck 1
    playlistComponent(&player1, &player2, &appState)  // Manages track selection and state
{
    // Associate playlist component with both decks
    playlistComponent.setDecks(&deckGUI1, &deckGUI2);

    // Load saved application state (previously played tracks & positions)
    loadApplicationState();

    // Set the size of the main window
    setSize(800, 600);

    // Request microphone permissions if required for recording
    if (RuntimePermissions::isRequired(RuntimePermissions::recordAudio)
        && !RuntimePermissions::isGranted(RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request(RuntimePermissions::recordAudio,
            [&](bool granted) { if (granted) setAudioChannels(2, 2); });
    }
    else
    {
        setAudioChannels(0, 2); // Set audio output to stereo (2 channels)
    }

    // Add UI components to the window
    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);
    addAndMakeVisible(playlistComponent);
    addAndMakeVisible(crossFaderAndIsolation);
    addAndMakeVisible(spinningWheel);

    // Register audio formats (MP3, WAV, etc.)
    formatManager.registerBasicFormats();

    // Prepare both players for playback with a buffer of 512 samples and 44.1kHz sample rate
    player1.prepareToPlay(512, 44100);
    player2.prepareToPlay(512, 44100);

    // Debug message
    DBG("MainComponent::resized");
}

// Destructor: Cleans up audio resources and saves state before exit
MainComponent::~MainComponent()
{
    shutdownAudio();  // Release all audio resources
    saveApplicationState();  // Save track positions and states
}

//==============================================================================
// Called before playback starts, setting sample rate and buffer size
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // Prepare both audio players
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // Add both players as input sources to the mixer
    mixerSource.addInputSource(&player1.getResampleSource(), false);
    mixerSource.addInputSource(&player2.getResampleSource(), false);
}

// Called to fill the audio buffer with the next block of audio data
void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);  // Mix audio from both decks
}

// Releases audio resources when playback stops
void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
}

//==============================================================================
// Paints the UI, setting background color and drawing text
void MainComponent::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));  // Set background color
    g.setColour(Colours::white);
    g.setFont(14.0f);
    g.drawText("PlaylistComponent", getLocalBounds(), Justification::centred, true);  // Draw label
}

// Saves the current track and playback position for both decks
void MainComponent::saveApplicationState()
{
    String track1 = "";
    double position1 = 0.0;

    // If player1 has a loaded track, save its position
    if (player1.getTrackLength() > 0)
    {
        position1 = player1.getPositionRelative();
    }

    String track2 = "";
    double position2 = 0.0;

    // If player2 has a loaded track, save its position
    if (player2.getTrackLength() > 0)
    {
        position2 = player2.getPositionRelative();
    }

    // Save the track states to `appState`
    appState.saveState(track1, position1, track2, position2);
}

// Loads the previously saved application state (track and position)
void MainComponent::loadApplicationState()
{
    String track1, track2;
    double position1, position2;

    // Retrieve saved state
    appState.loadState(track1, position1, track2, position2);

    // Load track 1 if it exists
    if (track1.isNotEmpty())
    {
        URL url1(track1);
        if (url1.isLocalFile())  // Ensure it's a valid file
        {
            player1.loadURL(url1);
            deckGUI1.loadURL(url1);
            player1.setPositionRelative(position1);  // Restore previous position
        }
    }

    // Load track 2 if it exists
    if (track2.isNotEmpty())
    {
        URL url2(track2);
        if (url2.isLocalFile())  // Ensure it's a valid file
        {
            player2.loadURL(url2);
            deckGUI2.loadURL(url2);
            player2.setPositionRelative(position2);  // Restore previous position
        }
    }
}

// Resizes and positions UI elements dynamically
void MainComponent::resized()
{
    int deckWidth = getWidth() / 2;  // Split window width between two decks
    int deckHeight = getHeight() * 0.6;  // Decks take up 60% of height
    int crossFaderAndIsolationHeight = 40;  // Fixed height for crossfader section

    // Set bounds for the deck GUI elements
    deckGUI1.setBounds(0, 0, deckWidth, deckHeight);
    deckGUI2.setBounds(deckWidth, 0, deckWidth, deckHeight);

    // Position crossfader below the decks
    crossFaderAndIsolation.setBounds(0, deckHeight, getWidth(), crossFaderAndIsolationHeight);

    // Place playlist component below the crossfader
    playlistComponent.setBounds(0, deckHeight + crossFaderAndIsolationHeight,
        getWidth(), getHeight() - deckHeight - crossFaderAndIsolationHeight);
}
