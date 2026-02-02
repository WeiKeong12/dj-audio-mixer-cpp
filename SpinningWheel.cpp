/*
  ==============================================================================

    SpinningWheel.cpp
    Created: 25 Feb 2025 4:09:09am
    Author:  Wei Keong

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SpinningWheel.h"
#include "DJAudioPlayer.h"

// Constructor for the SpinningWheel class, takes a DJAudioPlayer pointer to control playback
SpinningWheel::SpinningWheel(DJAudioPlayer* player) : player(player)
{
    startTimer(50); // Start the timer to update the spinning wheel every 50ms
}

// Destructor, stops the timer when the SpinningWheel object is destroyed
SpinningWheel::~SpinningWheel()
{
    stopTimer(); // Stop the timer
}

// Paint method for rendering the spinning wheel graphic
void SpinningWheel::paint(Graphics& g)
{
    // Fill the background with the chosen color for the window
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    // Set the color for the spinning wheel
    g.setColour(Colours::grey);

    // Apply a rotation transformation to the graphics context
    g.addTransform(AffineTransform::rotation(angle, getWidth() / 2.0f, getHeight() / 2.0f));

    // Define the radius of the spinning wheel
    float radius = jmin(getWidth(), getHeight()) / 2.5f;

    // Set the color for the outer circle and draw it
    g.setColour(Colours::black);
    g.fillEllipse(getWidth() / 2.0f - radius, getHeight() / 2.0f - radius, radius * 2, radius * 2);

    // Set the color for the border and draw the outer ellipse
    g.setColour(Colours::dimgrey);
    g.drawEllipse(getWidth() / 2.0f - radius, getHeight() / 2.0f - radius, radius * 2, radius * 2, 2.0f);

    // Set the color for the spinning line
    g.setColour(Colours::darkred);

    // Define the length of the line and its starting position
    float lineLength = radius * 0.9f;
    float x1 = getWidth() / 2.0f;
    float y1 = getHeight() / 2.0f;
    float x2 = x1;
    float y2 = y1 - lineLength;

    // Draw the line in the center of the wheel
    g.drawLine(x1, y1, x2, y2, 6.0f);

    // Set the color for the center circle and draw it
    float centerRadius = radius * 0.3f;
    g.setColour(Colours::white);
    g.fillEllipse(getWidth() / 2.0f - centerRadius, getHeight() / 2.0f - centerRadius, centerRadius * 2, centerRadius * 2);
}

// Resized method, not used but can be implemented for handling resizing events
void SpinningWheel::resized()
{
}

// Timer callback to update the spinning animation
void SpinningWheel::timerCallback()
{
    if (spinning)
    {
        // Increment the angle to make the wheel spin
        angle += 0.1f;

        // Keep the angle within the range of 0 to 2*PI to avoid overflow
        if (angle > MathConstants<float>::twoPi)
            angle -= MathConstants<float>::twoPi;

        // Request a repaint to update the spinning wheel's display
        repaint();
    }
}

// Start the spinning animation
void SpinningWheel::startSpinning()
{
    spinning = true; // Set the spinning flag to true
}

// Stop the spinning animation
void SpinningWheel::stopSpinning()
{
    spinning = false; // Set the spinning flag to false
    repaint(); // Request a repaint to stop the wheel's movement visually
}

// Handle mouse dragging for track position scrubbing
void SpinningWheel::mouseDrag(const MouseEvent& event)
{
    if (player == nullptr) return; // If player is null, do nothing

    // Get the track's length and current position from the player
    double trackLength = player->getTrackLength();
    double currentPosition = player->getPosition();

    // Get the drag distance in the Y direction
    double dragAmount = event.getDistanceFromDragStartY();

    // Define a scrub speed for how fast to move the track position based on the drag amount
    double scrubSpeed = 0.1;

    // Calculate the new position based on drag distance
    double newPosition = currentPosition - (dragAmount * scrubSpeed);

    // Limit the new position to be within the track's bounds
    newPosition = jlimit(0.0, trackLength, newPosition);

    // Set the new position in the player
    player->setPosition(newPosition);
}

// Reset the spinning wheel to its initial state
void SpinningWheel::resetWheel()
{
    stopSpinning(); // Stop spinning
    angle = 0; // Reset the rotation angle to 0
    repaint(); // Request a repaint to reset the visual appearance of the wheel
}