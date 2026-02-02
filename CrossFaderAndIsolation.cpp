/*
  ==============================================================================

    CrossFaderAndIsolation.cpp
    Created: 26 Feb 2025 4:14:23am
    Author:  Wei Keong

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "CrossFaderAndIsolation.h"

CrossFaderAndIsolation::CrossFaderAndIsolation(DJAudioPlayer* player1, DJAudioPlayer* player2)
    : player1(player1), player2(player2)
{
    // Configure the crossfade slider
    crossfadeSlider.setRange(0.0, 1.0, 0.01);    // Set range from 0 to 1 with 0.01 steps
    crossfadeSlider.setValue(0.5);               // Initialize to center position (equal volume)
    crossfadeSlider.setSliderStyle(Slider::LinearHorizontal);  // Horizontal slider style
    crossfadeSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);  // No text display
    crossfadeSlider.addListener(this);            // Register for slider value changes
    addAndMakeVisible(crossfadeSlider);           // Add slider to component and make it visible

    // Configure the isolation buttons
    isolateTrack1Button.setColour(TextButton::buttonColourId, Colours::grey);  // Initial button color
    isolateTrack2Button.setColour(TextButton::buttonColourId, Colours::grey);
    isolateTrack1Button.addListener(this);        // Register for button click events
    isolateTrack2Button.addListener(this);
    addAndMakeVisible(isolateTrack1Button);       // Add buttons to component and make them visible
    addAndMakeVisible(isolateTrack2Button);
}

// Destructor (currently empty as there's no dynamic memory to clean up)
CrossFaderAndIsolation::~CrossFaderAndIsolation()
{
}

// Paint method handles the visual appearance of the component
void CrossFaderAndIsolation::paint(Graphics& g)
{
    // Fill background with the application's background color
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    // Draw a grey border around the component
    g.setColour(Colours::grey);
    g.drawRect(getLocalBounds(), 1);
}

// Resized method handles the layout of child components when this component is resized
void CrossFaderAndIsolation::resized()
{
    // Get component dimensions
    int totalWidth = getWidth();
    int totalHeight = getHeight();

    // Calculate sizes for child components
    int sliderWidth = totalWidth / 2;
    int sliderHeight = totalHeight / 2;
    int buttonWidth = totalWidth / 6;
    int buttonHeight = sliderHeight;

    // Calculate positions to center the slider
    int centerX = (totalWidth - sliderWidth) / 2;
    int centerY = (totalHeight - sliderHeight) / 2;

    // Space between buttons and slider
    int buttonSpacing = 50;

    // Position the slider and buttons
    crossfadeSlider.setBounds(centerX, centerY, sliderWidth, sliderHeight);
    isolateTrack1Button.setBounds(centerX - buttonWidth - buttonSpacing, centerY, buttonWidth, buttonHeight);
    isolateTrack2Button.setBounds(centerX + sliderWidth + buttonSpacing, centerY, buttonWidth, buttonHeight);
}

// Called when a slider's value changes
void CrossFaderAndIsolation::sliderValueChanged(Slider* slider)
{
    // If either track is isolated, reset slider to center and exit
    if (track1Isolated || track2Isolated)
    {
        crossfadeSlider.setValue(0.5);
        return;
    }

    // Handle crossfade slider value changes
    if (slider == &crossfadeSlider)
    {
        double position = slider->getValue();
        // Adjust volume of both players based on slider position
        // When slider is at 0, player1 is at full volume and player2 is silent
        // When slider is at 1, player1 is silent and player2 is at full volume
        player1->setGain(1.0 - position);
        player2->setGain(position);
    }
}

// Called when a button is clicked
void CrossFaderAndIsolation::buttonClicked(Button* button)
{
    // Handle Track 1 isolation button
    if (button == &isolateTrack1Button)
    {
        // Toggle isolation state for track 1
        track1Isolated = !track1Isolated;
        track2Isolated = false;  // Ensure track 2 is not isolated

        if (track1Isolated)
        {
            // Isolate track 1: full volume for player1, mute player2
            player1->setGain(1.0);
            player2->setGain(0.0);
            // Visual feedback: make button red
            isolateTrack1Button.setColour(TextButton::buttonColourId, Colours::red);
            isolateTrack2Button.setColour(TextButton::buttonColourId, Colours::grey);
            // Disable crossfade slider during isolation
            crossfadeSlider.setEnabled(false);
        }
        else
        {
            // If turning off isolation, restore both players to full volume
            player1->setGain(1.0);
            player2->setGain(1.0);
            // Reset button color
            isolateTrack1Button.setColour(TextButton::buttonColourId, Colours::grey);
            // Re-enable crossfade slider
            crossfadeSlider.setEnabled(true);
        }
    }
    // Handle Track 2 isolation button
    else if (button == &isolateTrack2Button)
    {
        // Toggle isolation state for track 2
        track2Isolated = !track2Isolated;
        track1Isolated = false;  // Ensure track 1 is not isolated

        if (track2Isolated)
        {
            // Isolate track 2: mute player1, full volume for player2
            player1->setGain(0.0);
            player2->setGain(1.0);
            // Visual feedback: make button red
            isolateTrack2Button.setColour(TextButton::buttonColourId, Colours::red);
            isolateTrack1Button.setColour(TextButton::buttonColourId, Colours::grey);
            // Disable crossfade slider during isolation
            crossfadeSlider.setEnabled(false);
        }
        else
        {
            // If turning off isolation, restore both players to full volume
            player1->setGain(1.0);
            player2->setGain(1.0);
            // Reset button color
            isolateTrack2Button.setColour(TextButton::buttonColourId, Colours::grey);
            // Re-enable crossfade slider
            crossfadeSlider.setEnabled(true);
        }
    }

    // Reset crossfade slider to center position after any button action
    crossfadeSlider.setValue(0.5);
}