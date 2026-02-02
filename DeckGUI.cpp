/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 20 Feb 2025 3:14:45am
    Author:  Wei Keong

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"
#include "SpinningWheel.h"
#include "WaveformDisplay.h"

//==============================================================================
// Constructor initializes the deck UI with all its components and listeners
DeckGUI::DeckGUI(DJAudioPlayer* _player,
    AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse
) : player(_player),
waveformDisplay(formatManagerToUse, cacheToUse),
spinningWheel(_player)
{
    // Add playback control buttons to the component
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(pauseButton);

    // Add sliders for volume and position control
    addAndMakeVisible(volSlider);
    addAndMakeVisible(posSlider);

    // Add speed control buttons
    addAndMakeVisible(speed025Button);
    addAndMakeVisible(speed050Button);
    addAndMakeVisible(speed100Button);
    addAndMakeVisible(speed150Button);
    addAndMakeVisible(speed175Button);

    // Add visual components
    addAndMakeVisible(waveformDisplay);
    addAndMakeVisible(spinningWheel);

    // Register button listeners for playback controls
    playButton.addListener(this);
    stopButton.addListener(this);
    pauseButton.addListener(this);

    // Register slider listeners
    volSlider.addListener(this);
    posSlider.addListener(this);

    // Register button listeners for speed controls
    speed025Button.addListener(this);
    speed050Button.addListener(this);
    speed100Button.addListener(this);
    speed150Button.addListener(this);
    speed175Button.addListener(this);

    // Add and configure cue point controls
    addAndMakeVisible(cueButton);
    addAndMakeVisible(jumpCueButton);
    cueButton.addListener(this);
    jumpCueButton.addListener(this);

    // Configure volume slider range and initial value
    volSlider.setRange(0.0, 1.0);
    volSlider.setValue(1.0);

    // Set default speed button to green (1x speed)
    speed100Button.setColour(TextButton::buttonColourId, Colours::green);

    // Configure position slider range
    posSlider.setRange(0.0, 1.0);

    // Start timer for UI updates
    startTimer(500);
}

// Destructor ensures timer is stopped when component is destroyed
DeckGUI::~DeckGUI()
{
    stopTimer();
}

// Paints the component and its contents
void DeckGUI::paint(Graphics& g)
{
    // Fill background
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    // Draw border
    g.setColour(Colours::grey);
    g.drawRect(getLocalBounds(), 1);

    // Draw track name with scrolling effect
    g.setColour(Colours::cyan);
    String displayedText = trackName + "     ";
    g.drawText(displayedText, volSlider.getX() - textOffset, volSlider.getY() - 55,
        volSlider.getWidth() * 2, 20, Justification::left, true);

    // Draw slider labels
    g.setColour(Colours::red);
    g.drawText("Volume", volSlider.getX() - 75, volSlider.getY() - 5, volSlider.getWidth(), 20, Justification::centred, true);
    g.drawText("Track Minutes", posSlider.getX() - 57, posSlider.getY() - 5, posSlider.getWidth(), 20, Justification::centred, true);
}

// Handles component layout when resized
void DeckGUI::resized()
{
    // Calculate layout dimensions
    double rowH = getHeight() / 8;
    double buttonWidth = getWidth() / 5;
    double sliderWidth = getWidth() * 0.5;
    double wheelSize = getWidth() * 0.5;
    double buttonHeight = rowH * 0.7;

    // Position waveform display at the top
    waveformDisplay.setBounds(0, 0, getWidth(), rowH * 1.8);

    // Position sliders
    volSlider.setBounds(0, rowH * 3.8, sliderWidth, rowH);
    posSlider.setBounds(0, rowH * 5.0, sliderWidth, rowH);

    // Position spinning wheel animation
    spinningWheel.setBounds(sliderWidth, rowH * 1.9, wheelSize, wheelSize);

    // Calculate positions for button rows
    double firstRowY = rowH * 6.4;
    double secondRowY = firstRowY + buttonHeight + 2;

    // Position playback control buttons in first row
    playButton.setBounds(0, firstRowY, buttonWidth, buttonHeight);
    stopButton.setBounds(buttonWidth, firstRowY, buttonWidth, buttonHeight);
    pauseButton.setBounds(buttonWidth * 2, firstRowY, buttonWidth, buttonHeight);
    cueButton.setBounds(buttonWidth * 3, firstRowY, buttonWidth, buttonHeight);
    jumpCueButton.setBounds(buttonWidth * 4, firstRowY, buttonWidth, buttonHeight);

    // Calculate speed button width
    double speedButtonWidth = getWidth() / 5;

    // Position speed control buttons in second row
    speed025Button.setBounds(0, secondRowY, speedButtonWidth, buttonHeight);
    speed050Button.setBounds(speedButtonWidth, secondRowY, speedButtonWidth, buttonHeight);
    speed100Button.setBounds(speedButtonWidth * 2, secondRowY, speedButtonWidth, buttonHeight);
    speed150Button.setBounds(speedButtonWidth * 3, secondRowY, speedButtonWidth, buttonHeight);
    speed175Button.setBounds(speedButtonWidth * 4, secondRowY, speedButtonWidth, buttonHeight);
}

// Handles button click events
void DeckGUI::buttonClicked(Button* button)
{
    // Handle Play button
    if (button == &playButton)
    {
        std::cout << "Play button was clicked" << std::endl;
        if (isSongLoaded)
        {
            // Start playback and spinning animation
            player->start();
            spinningWheel.startSpinning();

            // Update state and start timer for UI updates
            isPlaying = true;
            startTimer(50);
        }
        else
        {
            std::cout << "No song loaded! Load a song first." << std::endl;
        }
    }

    // Handle Stop button
    if (button == &stopButton)
    {
        std::cout << "Stop button was clicked" << std::endl;

        // Reset playback position to start
        player->setPositionRelative(0.0);
        player->stop();
        posSlider.setValue(0.0);
        spinningWheel.stopSpinning();

        // Update state and stop timer
        isPlaying = false;
        stopTimer();
        textOffset = 0;
        repaint();
    }

    // Handle Pause button
    if (button == &pauseButton)
    {
        std::cout << "Pause button was clicked" << std::endl;

        // Pause playback and stop spinning animation
        player->pause();
        spinningWheel.stopSpinning();

        // Update state and stop timer
        isPlaying = false;
        stopTimer();
    }

    // Handle speed buttons
    else if (button == &speed025Button)
    {
        setSpeedAndUpdateButtons(0.25, &speed025Button);
    }
    else if (button == &speed050Button)
    {
        setSpeedAndUpdateButtons(0.5, &speed050Button);
    }
    else if (button == &speed100Button)
    {
        setSpeedAndUpdateButtons(1.0, &speed100Button);
    }
    else if (button == &speed150Button)
    {
        setSpeedAndUpdateButtons(1.5, &speed150Button);
    }
    else if (button == &speed175Button)
    {
        setSpeedAndUpdateButtons(1.75, &speed175Button);
    }

    // Handle cue point buttons
    if (button == &cueButton)
    {
        // Set cue point at current position
        player->setCuePoint();
        waveformDisplay.setCuePoint(player->getCuePoint());
    }
    else if (button == &jumpCueButton)
    {
        // Jump to previously set cue point
        player->jumpToCue();
    }
}

// Handles slider value changes
void DeckGUI::sliderValueChanged(Slider* slider)
{
    if (slider == &volSlider)
    {
        // Update volume level
        player->setGain(slider->getValue());
    }
    else if (slider == &posSlider)
    {
        // Update playback position
        player->setPositionRelative(slider->getValue());
    }
}

// Determines if the component accepts dropped files
bool DeckGUI::isInterestedInFileDrag(const StringArray& files)
{
    std::cout << "DeckGUI::isInterestedInFileDrag" << std::endl;
    return true; // Accept all file types
}

// Handles dropped files
void DeckGUI::filesDropped(const StringArray& files, int x, int y)
{
    std::cout << "DeckGUI::filesDropped" << std::endl;
    if (files.size() == 1)
    {
        // Load the dropped file
        player->loadURL(URL{ File{files[0]} });
    }
}

// Timer callback for regular UI updates
void DeckGUI::timerCallback() {
    std::cout << "DeckGUI::timerCallback" << std::endl;

    // Get and update current playback position
    float trackPosition = player->getPositionRelative();
    waveformDisplay.setPositionRelative(trackPosition);

    // Update scrolling text animation if playing
    if (isPlaying) {
        textOffset += 2;
        if (textOffset > volSlider.getWidth()) {
            textOffset = 0;
        }
        repaint();
    }

    // Check if track has finished playing
    if (trackPosition >= 1.0) {
        std::cout << "Track finished. Resetting to start." << std::endl;
        player->setPositionRelative(0.0);
        posSlider.setValue(0.0);
        spinningWheel.stopSpinning();
    }
}

// Loads an audio file from a URL
void DeckGUI::loadURL(const URL& audioURL) {
    // Load the audio in the player and update waveform display
    player->loadURL(audioURL);
    waveformDisplay.loadURL(audioURL);
    isSongLoaded = true;

    // Extract and store track name for display
    trackName = audioURL.getLocalFile().getFileNameWithoutExtension();
    textOffset = 0;

    // Reset speed to normal (1x)
    setSpeedAndUpdateButtons(1.0, &speed100Button);

    // Reset playback state
    spinningWheel.stopSpinning();
    posSlider.setValue(0.0);

    // Reset visual components
    spinningWheel.resetWheel();
    waveformDisplay.setPositionRelative(0.0);

    // Refresh the display
    repaint();

    std::cout << "New song loaded: " << trackName << std::endl;
}

// Helper method to set playback speed and update button appearances
void DeckGUI::setSpeedAndUpdateButtons(double speed, Button* activeButton)
{
    // Reset all speed buttons to default color
    speed025Button.setColour(TextButton::buttonColourId, getLookAndFeel().findColour(TextButton::buttonColourId));
    speed050Button.setColour(TextButton::buttonColourId, getLookAndFeel().findColour(TextButton::buttonColourId));
    speed100Button.setColour(TextButton::buttonColourId, getLookAndFeel().findColour(TextButton::buttonColourId));
    speed150Button.setColour(TextButton::buttonColourId, getLookAndFeel().findColour(TextButton::buttonColourId));
    speed175Button.setColour(TextButton::buttonColourId, getLookAndFeel().findColour(TextButton::buttonColourId));

    // Highlight the active speed button
    activeButton->setColour(TextButton::buttonColourId, Colours::green);

    // Set the actual playback speed
    player->setSpeed(speed);
    std::cout << "Speed set to " << speed << "x" << std::endl;
}