/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 20 Feb 2025 5:57:25am
    Author:  Wei Keong

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"
#include "DJAudioPlayer.h" 
#include "AppState.h"
#include "DeckGUI.h"

TextButton* activeTrack1Button = nullptr;
TextButton* activeTrack2Button = nullptr;

//==============================================================================
// Constructor for PlaylistComponent
PlaylistComponent::PlaylistComponent(DJAudioPlayer* _player1, DJAudioPlayer* _player2, AppState* _appState)
    : player1(_player1), player2(_player2), appState(_appState)
{
    // Register the basic audio formats for handling tracks
    formatManager.registerBasicFormats();

    // Define columns in the table view (Track Title, Duration, Track 1, Track 2, Delete)
    tableComponent.getHeader().addColumn("Track title", 1, 400);
    tableComponent.getHeader().addColumn("Duration", 2, 80);
    tableComponent.getHeader().addColumn("Track 1", 3, 100);
    tableComponent.getHeader().addColumn("Track 2", 4, 100);
    tableComponent.getHeader().addColumn("Delete", 5, 80);

    // Set the current component model to this object
    tableComponent.setModel(this);

    // Make the table and load button visible in the component
    addAndMakeVisible(tableComponent);
    addAndMakeVisible(loadButton);

    // Add button listener
    loadButton.addListener(this);

    // Load state from AppState if available
    if (appState != nullptr)
    {
        loadState();
    }
}

// Destructor for PlaylistComponent
PlaylistComponent::~PlaylistComponent()
{
    // Save the state to AppState when this component is destroyed
    if (appState != nullptr)
    {
        saveState();
    }
}

// Paint method to render the background and title text
void PlaylistComponent::paint(juce::Graphics& g)
{
    // Set background color and draw border
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);

    // Set font and draw component title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(14.0f));
    g.drawText("PlaylistComponent", getLocalBounds(),
        juce::Justification::centred, true);
}

// Resizing method to adjust the layout based on the component's size
void PlaylistComponent::resized()
{
    int buttonWidth = 100;
    int buttonHeight = 25;
    int margin = 10;

    // Resize the table component and button
    tableComponent.setBounds(0, 0, getWidth(), getHeight() - buttonHeight - margin);
    loadButton.setBounds(getWidth() - buttonWidth - margin,
        getHeight() - buttonHeight - margin + 3,
        buttonWidth, buttonHeight);
}

// Returns the number of rows in the table (equal to number of tracks)
int PlaylistComponent::getNumRows()
{
    return trackTitles.size();
}

// Paint the row background, alternating between two colors for even/odd rows
void PlaylistComponent::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(Colours::orange);  // Highlight selected row
    }
    else
    {
        g.fillAll((rowNumber % 2 == 0) ? Colours::darkgreen : Colours::darkgrey);
    }
}

// Paint the cell contents, such as track title, duration, and buttons
void PlaylistComponent::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    g.setColour(Colours::black);
    g.setFont(14.0f);

    if (rowNumber < trackTitles.size())
    {
        // Handle different columns (Title, Duration, Track 1, Track 2, Delete)
        if (columnId == 1)
        {
            g.drawText(trackTitles[rowNumber], 0, 0, width, height, Justification::left, true);
        }
        else if (columnId == 2)
        {
            if (rowNumber < trackDurations.size())
            {
                g.drawText(trackDurations[rowNumber], 0, 0, width, height, Justification::centred, true);
            }
            else
            {
                g.drawText("00:00:00", 0, 0, width, height, Justification::centred, true);
            }
        }
        else if (columnId == 3 || columnId == 4)
        {
            g.drawText(columnId == 3 ? "Track 1" : "Track 2", 0, 0, width, height, Justification::centred, true);
        }
        else if (columnId == 5)
        {
            g.drawText("Delete", 0, 0, width, height, Justification::centred, true);
        }
    }
}

// Refresh the components (buttons) for specific cells
Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate)
{
    if (columnId == 3 || columnId == 4)  // Load buttons for Track 1/Track 2
    {
        TextButton* btn = dynamic_cast<TextButton*>(existingComponentToUpdate);
        if (btn == nullptr)
        {
            btn = new TextButton(columnId == 3 ? "Load 1" : "Load 2");
            btn->addListener(this);
            btn->setColour(TextButton::buttonColourId, getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
            btn->setColour(TextButton::textColourOffId, Colours::white);
        }
        btn->setComponentID(String(rowNumber) + (columnId == 3 ? "_1" : "_2"));
        return btn;
    }
    else if (columnId == 5)  // Delete button
    {
        TextButton* deleteBtn = dynamic_cast<TextButton*>(existingComponentToUpdate);
        if (deleteBtn == nullptr)
        {
            deleteBtn = new TextButton("Delete");
            deleteBtn->addListener(this);
            deleteBtn->setComponentID("del_" + String(rowNumber));
            deleteBtn->addMouseListener(this, true);
        }
        return deleteBtn;
    }
    return existingComponentToUpdate;
}

// Button click handling (load button, delete button, and load buttons for tracks)
void PlaylistComponent::buttonClicked(Button* button)
{
    if (button == &loadButton)  // Load button clicked
    {
        auto fileChooserFlags = FileBrowserComponent::canSelectFiles;
        fChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
            {
                File chosenFile = chooser.getResult();
                if (chosenFile.existsAsFile())
                {
                    String trackName = chosenFile.getFileNameWithoutExtension();
                    trackTitles.push_back(trackName);
                    loadURL(URL{ chosenFile });
                }
            });
    }
    else  // Handle track load or delete button
    {
        String id = button->getComponentID();

        if (id.startsWith("del_"))  // Delete track
        {
            int trackIndex = id.fromFirstOccurrenceOf("del_", false, false).getIntValue();
            if (trackIndex >= 0 && trackIndex < trackTitles.size())
            {
                trackTitles.erase(trackTitles.begin() + trackIndex);
                trackDurations.erase(trackDurations.begin() + trackIndex);
                trackURLs.erase(trackURLs.begin() + trackIndex);
                trackPathStrings.erase(trackPathStrings.begin() + trackIndex);
                tableComponent.updateContent();
                saveState();
            }
        }
        else  // Load track into appropriate deck
        {
            int trackIndex = id.upToFirstOccurrenceOf("_", false, false).getIntValue();
            bool isTrack1 = id.contains("_1");

            if (trackIndex >= 0 && trackIndex < trackURLs.size())
            {
                URL trackURL = trackURLs[trackIndex];

                if (isTrack1 && player1 != nullptr)
                {
                    player1->loadURL(trackURL);
                    if (deck1 != nullptr) deck1->loadURL(trackURL);
                    if (activeTrack1Button != nullptr)
                    {
                        activeTrack1Button->setColour(TextButton::buttonColourId,
                            getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
                    }

                    activeTrack1Button = dynamic_cast<TextButton*>(button);
                    activeTrack1Button->setColour(TextButton::buttonColourId, Colours::red);
                }
                else if (!isTrack1 && player2 != nullptr)
                {
                    player2->loadURL(trackURL);
                    if (deck2 != nullptr) deck2->loadURL(trackURL);
                    if (activeTrack2Button != nullptr)
                    {
                        activeTrack2Button->setColour(TextButton::buttonColourId,
                            getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
                    }
                    activeTrack2Button = dynamic_cast<TextButton*>(button);
                    activeTrack2Button->setColour(TextButton::buttonColourId, Colours::red);
                }
            }
        }
    }
}

// Mouse enter and exit for handling hover effects on delete buttons
void PlaylistComponent::mouseEnter(const MouseEvent& event)
{
    TextButton* button = dynamic_cast<TextButton*>(event.eventComponent);
    if (button && button->getComponentID().startsWith("del_"))
    {
        button->setColour(TextButton::buttonColourId, Colours::red);
    }
}

void PlaylistComponent::mouseExit(const MouseEvent& event)
{
    TextButton* button = dynamic_cast<TextButton*>(event.eventComponent);
    if (button && button->getComponentID().startsWith("del_"))
    {
        button->setColour(TextButton::buttonColourId, getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    }
}

// Load the URL into the transport source and calculate the track's duration
void PlaylistComponent::loadURL(URL audioURL)
{
    auto inputStream = audioURL.createInputStream(false);
    if (!inputStream)
    {
        DBG("Error: Could not open file: " + audioURL.toString(false));
        return;
    }

    std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(std::move(inputStream)));
    if (!reader)
    {
        DBG("Error: Could not create reader for: " + audioURL.toString(false));
        return;
    }

    auto transportSource = std::make_unique<AudioTransportSource>();
    auto readerSource = std::make_unique<AudioFormatReaderSource>(reader.get(), true);
    transportSource->setSource(readerSource.get(), 0, nullptr, reader->sampleRate);

    // Calculate track duration and format it into hours:minutes:seconds
    double totalSeconds = reader->lengthInSamples / reader->sampleRate;
    int hours = static_cast<int>(totalSeconds) / 3600;
    int minutes = (static_cast<int>(totalSeconds) % 3600) / 60;
    int seconds = static_cast<int>(totalSeconds) % 60;

    // Store track data
    trackURLs.push_back(audioURL);
    trackDurations.push_back(String::formatted("%02d:%02d:%02d", hours, minutes, seconds));
    transportSources.push_back(std::move(transportSource));
    readerSources.push_back(std::move(readerSource));

    trackPathStrings.push_back(audioURL.toString(false));

    tableComponent.updateContent();
    repaint();
    saveState();
}

// Save the current playlist state to AppState
void PlaylistComponent::saveState()
{
    if (appState == nullptr) return;

    appState->savePlaylist(trackTitles, trackPathStrings, trackDurations);
}

// Load the playlist state from AppState
void PlaylistComponent::loadState()
{
    if (appState == nullptr) return;
    trackTitles.clear();
    trackDurations.clear();
    trackURLs.clear();
    trackPathStrings.clear();

    std::vector<juce::String> savedTitles, savedPaths, savedDurations;
    appState->loadPlaylist(savedTitles, savedPaths, savedDurations);

    for (size_t i = 0; i < savedTitles.size(); ++i)
    {
        trackTitles.push_back(savedTitles[i]);
        trackDurations.push_back(savedDurations[i]);
        trackURLs.push_back(URL{ savedPaths[i] });
        trackPathStrings.push_back(savedPaths[i]);
    }

    tableComponent.updateContent();
}