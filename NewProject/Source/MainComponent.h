#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component,
                       private juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    
    juce::Image backgroundImage;
    //juce::Image backgroundImage;  
    // Stores your background image
    
    //Visually adds the buttons
    bool rainbowEnabled = true;       // For toggle state
    juce::TextButton rainbowButton { "Toggle Rainbow" };
    juce::TextButton clearButton { "Clear" };
    juce::TextButton saveButton { "Save" };
    juce::TextButton openButton { "Open" };


    
    float rainbowPhase = 0.0f;        // animation phase [0, 1)

    void timerCallback() override;
    
    juce::TextEditor inputEditor;
    juce::StringArray messages;
    
    static constexpr int linesPerPage = 32;
    int currentPage = 0;
    int getTotalPages() const;
    
    juce::String statusMessage;
    int statusTimer = 0;

    
    bool keyPressed (const juce::KeyPress& key) override;
    std::unique_ptr<juce::FileChooser> fileChooser;



    


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
