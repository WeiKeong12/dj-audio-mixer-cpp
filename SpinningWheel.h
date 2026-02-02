/*
  ==============================================================================

    SpinningWheel.h
    Created: 25 Feb 2025 4:09:09am
    Author:  Wei Keong

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"

class SpinningWheel : public Component, public Timer
{
public:
    SpinningWheel(DJAudioPlayer* player);

    // Destructor: Cleans up resources (stop timer if active)
    ~SpinningWheel() override;

    // Paint method: Responsible for rendering the spinning wheel graphic
    void paint(Graphics&) override;

    // Resized method: Called when the component is resized (not used in this case)
    void resized() override;

    // Timer callback: Updates the spinning wheel's angle for continuous rotation
    void timerCallback() override;
    void startSpinning();
    void stopSpinning();

    // Reset the wheel's angle and stop spinning
    void resetWheel();
    void mouseDrag(const MouseEvent& event) override;

private:
    // Private member variables
    float angle = 0.0f;      // The current rotation angle of the spinning wheel
    bool spinning = false;   // Flag to track if the wheel is currently spinning
    DJAudioPlayer* player;   // Pointer to the DJAudioPlayer to control audio playback
};
