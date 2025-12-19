#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 600);
    
    setWantsKeyboardFocus (true);

    
    setOpaque (true);
    
    startTimerHz (60);

    backgroundImage = juce::ImageFileFormat::loadFrom (
        BinaryData::image_bg_jpeg,
        BinaryData::image_bg_jpegSize
        
    );
    
    addAndMakeVisible (openButton);

    openButton.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    openButton.setColour (juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
    openButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    openButton.setColour (juce::TextButton::textColourOnId,  juce::Colours::white);

    openButton.onClick = [this]
    {
         fileChooser = std::make_unique<juce::FileChooser>(
        "Open lyrics",
        juce::File ("/home/bederib/Desktop/NewProject/Rhymes"),
        "*.txt"
    );

    fileChooser->launchAsync (
        juce::FileBrowserComponent::openMode
        | juce::FileBrowserComponent::canSelectFiles,

        [this] (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (! file.existsAsFile())
                return;

            auto text = file.loadFileAsString();

            messages.clear();
            messages.addLines (text);
            currentPage = 0;

            statusMessage = "Loaded: " + file.getFileName();
            statusTimer   = 120;

            repaint();
        }
    );
    };

    
    
    addAndMakeVisible (saveButton);

    saveButton.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    saveButton.setColour (juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
    saveButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    saveButton.setColour (juce::TextButton::textColourOnId,  juce::Colours::white);

    saveButton.onClick = [this]
    {
    juce::File rhymesDir ("/home/bederib/Desktop/NewProject/Rhymes");
    if (! rhymesDir.exists())
        rhymesDir.createDirectory();

    auto fileName = "lyrics_" +
        juce::Time::getCurrentTime().toString (true, true)
            .replaceCharacters (": ", "__") + ".txt";

    juce::File outFile = rhymesDir.getChildFile (fileName);

    juce::String fullText;
    for (auto& line : messages)
        fullText << line << juce::newLine;

    outFile.replaceWithText (fullText);

    statusMessage = "Saved: " + outFile.getFileName();
    statusTimer = 120; // ~2 seconds
};

    
    addAndMakeVisible (rainbowButton);
    rainbowButton.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    rainbowButton.setColour (juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
    rainbowButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    rainbowButton.setColour (juce::TextButton::textColourOnId,  juce::Colours::white);

    rainbowButton.onClick = [this]
    {
        rainbowEnabled = ! rainbowEnabled;
        repaint(); // Redraw the screen when toggled
    };
    
    
    
    //clearButton.onClick = [this];
    addAndMakeVisible (clearButton);
    clearButton.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    clearButton.setColour (juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
    clearButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    clearButton.setColour (juce::TextButton::textColourOnId,  juce::Colours::white);


            clearButton.onClick = [this]{
    
            messages.clear();
            repaint();
        };
    addAndMakeVisible (inputEditor);
    inputEditor.setMultiLine (false);
    inputEditor.setReturnKeyStartsNewLine (false);
    inputEditor.setTextToShowWhenEmpty ("Type here and press Enter...", juce::Colours::grey);

    inputEditor.onReturnKey = [this]
    {
        
        auto text = inputEditor.getText().trim();

        if (text.isNotEmpty())
        {
            messages.add (text);   // store message
            inputEditor.clear();   // clear box for next one
            
            currentPage = juce::jmax (0, getTotalPages() - 1);
            repaint();             // redraw with new text
            
        }
    };
        
    
}

int MainComponent::getTotalPages() const
    {
        return (messages.size() + linesPerPage - 1) / linesPerPage;
    }

bool MainComponent::keyPressed (const juce::KeyPress& key)
{
    if (key == juce::KeyPress::leftKey)
    {
        if (currentPage > 0)
            --currentPage;

        repaint();
        return true;
    }

    if (key == juce::KeyPress::rightKey)
    {
        if (currentPage < getTotalPages() - 1)
            ++currentPage;

        repaint();
        return true;
    }

    return false;
}



MainComponent::~MainComponent()
{
}


//==============================================================================
// Timer callback – advances the rainbow animation
//

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    
    if (backgroundImage.isValid())
    {
        g.drawImage (backgroundImage,
                     getLocalBounds().toFloat(),
                     juce::RectanglePlacement::stretchToFit);
                     g.setFont (juce::FontOptions (16.0f));
                     g.setColour (juce::Colours::red);
                     g.drawText ("I love Lina and pokemon", getLocalBounds(), juce::Justification::centred, true);
    }
    else
    {
        g.fillAll (juce::Colours::black); // fallback
    }
      if (rainbowEnabled)
    {

    auto bounds = getLocalBounds().toFloat();

    // rainbowPhase is 0..1 – use it to pulse brightness & alpha
    float t = (std::sin (rainbowPhase * juce::MathConstants<float>::twoPi) + 1.0f) * 0.5f; // 0..1

    auto darkRed   = juce::Colour::fromFloatRGBA (0.3f, 0.0f, 0.0f, 0.9f);
    auto brightRed = juce::Colour::fromFloatRGBA (1.0f, 0.2f, 0.2f, 0.9f);
    auto orange    = juce::Colour::fromFloatRGBA (1.0f, 0.5f, 0.2f, 0.9f);

    // Lerp between dark and bright based on t
    auto c1 = darkRed.interpolatedWith (brightRed, t);
    auto c2 = brightRed.interpolatedWith (orange,   t);

    juce::ColourGradient grad (c1,
                               bounds.getX(), bounds.getY(),
                               c2,
                               bounds.getRight(), bounds.getBottom(),
                               false);

    g.setGradientFill (grad);
    g.setOpacity (0.6f);            // nice strong red wash
    g.fillRect (getLocalBounds());
    g.setOpacity (1.0f);
    }
    
    //========================================================================
    
     // 3. Draw messages on top of background + rainbow, under components
    g.setColour (juce::Colours::white);
    auto textArea = getLocalBounds().reduced (10);
    textArea.removeFromLeft (20);

    // leave space for INPUT at the bottom
    textArea.removeFromBottom (50);

    int lineHeight = 20;

    int startIndex = currentPage * linesPerPage;
    int endIndex   = juce::jmin (startIndex + linesPerPage, messages.size());

    for (int i = startIndex; i < endIndex; ++i)
    {
        auto lineBounds = textArea.removeFromTop (lineHeight);
        g.drawFittedText (messages[i],
                        lineBounds,
                        juce::Justification::left,
                        1);
    }
    
    g.setColour (juce::Colours::lightgrey);

    g.drawText ("Page " + juce::String (currentPage + 1) +
                " / " + juce::String (juce::jmax (1, getTotalPages())),
                10,
                getHeight() - 25,
                getWidth() - 20,
                20,
                juce::Justification::centred);
               
                if (statusTimer > 0)
{
    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.drawText (statusMessage,
                10,
                getHeight() - 45,
                getWidth() - 20,
                20,
                juce::Justification::centred);
            }
            
}




    


void MainComponent::resized()
{
    
   
    
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
   
    auto area   = getLocalBounds().reduced (10);
    auto open = area.removeFromBottom (15).removeFromRight (60);
    openButton.setBounds (open);

    
    
    auto bottomRow = area.removeFromBottom (30).removeFromLeft(200);
    inputEditor.setBounds (bottomRow);
    auto button = area.removeFromBottom (15).removeFromRight (130);
    rainbowButton.setBounds (button);
    auto topRow = area.removeFromBottom (10).removeFromRight(100);
    clearButton.setBounds (topRow);
    auto save = area.removeFromBottom (15).removeFromRight (60);
    saveButton.setBounds (save);

 
}


void MainComponent::timerCallback()
{
        if (rainbowEnabled)
    {
        rainbowPhase += 0.01f;
        if (rainbowPhase >= 1.0f)
            rainbowPhase -= 1.0f;
    }

    if (statusTimer > 0)
        --statusTimer;

    repaint();
}

