/*
  ==============================================================================

    PlaylistComponent.h
    Created: 20 Feb 2025 5:57:25am
    Author:  Wei Keong

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "AppState.h"
#include <vector>
#include <string>

class DeckGUI;

// PlaylistComponent class definition, inheriting from multiple JUCE classes
class PlaylistComponent : public juce::Component,
    public TableListBoxModel,  // For table component functionality
    public Button::Listener,   // For handling button clicks
    public MouseListener       // For handling mouse enter/exit events
{
public:
    // Constructor that accepts DJAudioPlayer pointers and an optional AppState pointer
    PlaylistComponent(DJAudioPlayer* player1, DJAudioPlayer* player2, AppState* appState = nullptr);
    ~PlaylistComponent() override; // Destructor

    // JUCE Component methods
    void paint(juce::Graphics&) override; // Paint the component
    void resized() override;             // Handle resizing of the component

    // TableListBoxModel methods to manage table data
    int getNumRows() override;
    void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    // Handle updates for cells in the table (e.g., creating buttons)
    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;

    // Button listener method for when a button is clicked
    void buttonClicked(Button* button) override;

    // Load track into a Deck
    void loadIntoDeck(DeckGUI* deck, const URL& audioURL);

    // Set pointers to the deck components
    void setDecks(DeckGUI* _deck1, DeckGUI* _deck2) {
        deck1 = _deck1;
        deck2 = _deck2;
    }

    // Save and load the playlist state
    void saveState();
    void loadState();

    // Mouse enter/exit event handling for buttons
    void mouseEnter(const MouseEvent& event) override;
    void mouseExit(const MouseEvent& event) override;

private:
    TableListBox tableComponent;          // Table component for displaying the playlist
    AudioFormatManager formatManager;      // Audio format manager for loading audio
    std::vector<std::unique_ptr<AudioFormatReaderSource>> readerSources;  // Audio readers for loaded tracks
    std::vector<std::unique_ptr<AudioTransportSource>> transportSources; // Audio transport sources for playback
    std::vector<juce::String> trackTitles; // List of track titles
    std::vector<juce::String> trackDurations; // List of track durations (formatted as strings)
    std::map<int, TextButton*> activeButtonsPerRow; // Mapping of active buttons for each row
    std::vector<URL> trackURLs;  // List of URLs for the audio tracks
    std::vector<juce::String> trackPathStrings; // List of track file paths as strings

    TextButton loadButton{ "LOAD" };  // Button for loading tracks
    FileChooser fChooser{ "Select a file..." };  // File chooser dialog for selecting tracks
    DJAudioPlayer* player1;  // Pointer to the first DJ audio player
    DJAudioPlayer* player2;  // Pointer to the second DJ audio player

    int activeTrack1Index = 0; // Index of the active track for player 1
    int activeTrack2Index = 0; // Index of the active track for player 2
    void loadURL(URL audioURL); // Method to load a track from a URL

    DeckGUI* deck1 = nullptr; // Pointer to the first deck GUI
    DeckGUI* deck2 = nullptr; // Pointer to the second deck GUI
    AppState* appState; // Pointer to the app state (for saving/loading playlist)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistComponent) // Ensures the component can't be copied and avoids memory leaks
};
