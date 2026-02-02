/*
  ==============================================================================

    CrossFaderAndIsolation.h
    Created: 26 Feb 2025 4:14:23am
    Author:  Wei Keong

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"

class CrossFaderAndIsolation : public Component, public Slider::Listener, public Button::Listener
{
public:
    // Constructor takes pointers to two DJAudioPlayer instances that will be controlled
    CrossFaderAndIsolation(DJAudioPlayer* player1, DJAudioPlayer* player2);

    // Destructor
    ~CrossFaderAndIsolation() override;

    // Component overrides for drawing and layout
    void paint(Graphics& g) override;
    void resized() override;

    // Listener interface implementations for user interaction
    void sliderValueChanged(Slider* slider) override;  // Called when slider value changes
    void buttonClicked(Button* button) override;       // Called when button is clicked

private:
    // Pointers to the audio players being controlled
    DJAudioPlayer* player1;
    DJAudioPlayer* player2;

    // UI controls
    Slider crossfadeSlider;                     // Slider for crossfading between tracks
    TextButton isolateTrack1Button{ "Isolate 1" }; // Button to isolate track 1
    TextButton isolateTrack2Button{ "Isolate 2" }; // Button to isolate track 2

    // State tracking
    bool track1Isolated = false;  // Tracks whether track 1 is currently isolated
    bool track2Isolated = false;  // Tracks whether track 2 is currently isolated
};
