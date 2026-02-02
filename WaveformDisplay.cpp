/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 20 Feb 2025 4:44:21am
    Author:  Wei Keong

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformDisplay.h"

WaveformDisplay::WaveformDisplay(AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse) :
    audioThumb(1000, formatManagerToUse, cacheToUse),
    fileLoaded(false),
    position(0)
{
    audioThumb.addChangeListener(this);  // Add this component as a listener to the audio thumbnail
}

// Destructor: Destructor for cleanup (not needed here as there are no resources to free explicitly)
WaveformDisplay::~WaveformDisplay()
{
}

// Paint method: Responsible for rendering the waveform display
void WaveformDisplay::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));  // Fill background with window's background color
    g.setColour(Colours::grey);  // Set color to grey for drawing border
    g.drawRect(getLocalBounds(), 1);  // Draw a border around the display

    if (fileLoaded)  // Check if file is loaded successfully
    {
        g.setColour(Colour::fromRGB(0, 255, 255).withAlpha(0.7f));  // Set color for waveform (cyan with transparency)
        audioThumb.drawChannel(g,
            getLocalBounds(),
            0,
            audioThumb.getTotalLength(),
            0,
            1.0f
        );  // Draw the waveform of the audio file

        g.setColour(Colours::ghostwhite);  // Set color for position line
        g.drawLine(position * getWidth(), 0, position * getWidth(), getHeight(), 3.0f);  // Draw position line

        if (cuePoint >= 0 && audioThumb.getTotalLength() > 0)  // Check if cue point is valid
        {
            float cueX = (cuePoint / audioThumb.getTotalLength()) * getWidth();  // Calculate the X position for the cue point
            g.setColour(Colours::red);  // Set color to red for the cue point
            g.drawLine(cueX, 0, cueX, getHeight(), 2.0f);  // Draw cue point line
        }
    }
    else  // If no file is loaded
    {
        g.setFont(20.0f);  // Set font size
        g.drawText("File not loaded...", getLocalBounds(),
            Justification::centred, true);  // Display message indicating file is not loaded
    }
}

// Resized method: Called when the component is resized (not used in this case)
void WaveformDisplay::resized()
{
}

// Load URL: Loads a new audio file from a URL and resets the waveform display
void WaveformDisplay::loadURL(URL audioURL)
{
    audioThumb.clear();  // Clear the previous audio thumbnail
    fileLoaded = audioThumb.setSource(new URLInputSource(audioURL));  // Set the new audio source

    if (fileLoaded)  // If the file loaded successfully
    {
        std::cout << "Waveform display reset and new track loaded!" << std::endl;  // Log success message
        position = 0;  // Reset position to the beginning of the track
        repaint();  // Repaint the waveform display
    }
    else  // If the file failed to load
    {
        std::cout << "Waveform failed to load." << std::endl;  // Log failure message
    }
}

// Change listener callback: Called when the audio thumbnail changes (e.g., new audio is loaded)
void WaveformDisplay::changeListenerCallback(ChangeBroadcaster* source)
{
    std::cout << "wfd: change received! " << std::endl;  // Log change received
    repaint();  // Repaint the waveform display to reflect changes
}

// Set position relative: Updates the position of the playback relative to the waveform display
void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position)  // Only update if the position has changed
    {
        position = pos;  // Update position
        repaint();  // Repaint the display to reflect the new position
    }
}

// Set cue point: Sets a cue point for the track and updates the display
void WaveformDisplay::setCuePoint(double newCuePoint)
{
    if (fileLoaded && newCuePoint >= 0 && newCuePoint <= audioThumb.getTotalLength())  // Validate cue point
    {
        cuePoint = newCuePoint;  // Set the new cue point
        repaint();  // Repaint the display to show the cue point
    }
}