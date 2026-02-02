/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 20 Feb 2025 4:44:21am
    Author:  Wei Keong

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class WaveformDisplay    : public Component, 
                           public ChangeListener  // Inherits from Component and ChangeListener
{
public:
    WaveformDisplay( AudioFormatManager & 	formatManagerToUse,
                    AudioThumbnailCache & 	cacheToUse );
    
    // Destructor: Clean up resources (not explicitly needed here)
    ~WaveformDisplay();

    // Paint function: Called to render the waveform display
    void paint (Graphics&) override;
    
    // Resized function: Called when the component is resized (currently does nothing)
    void resized() override;
    
    // ChangeListener callback: Called when the audio thumbnail changes (e.g., new audio loaded)
    void changeListenerCallback (ChangeBroadcaster *source) override;
    void loadURL(URL audioURL);
    void setPositionRelative(double pos);
    void setCuePoint(double pos);

private:
    AudioThumbnail audioThumb;  // AudioThumbnail object to hold the waveform
    bool fileLoaded;  // Flag to track if the audio file is loaded successfully
    double position;  // Current playback position (relative to the waveform display)
    double cuePoint = -1.0;  // Cue point position (initially unset)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};