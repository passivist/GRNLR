/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
GrrnlrrAudioProcessorEditor::GrrnlrrAudioProcessorEditor (GrrnlrrAudioProcessor& p)
    : AudioProcessorEditor (&p), Thread("Audio loading thread"), processor (p)
{
    addAndMakeVisible(openButton);
    openButton.setButtonText("Open...");
    openButton.addListener(this);
    
    // Position
    addAndMakeVisible(positionSlider = new ParameterSlider (*p.positionParam));
    positionSlider->setSliderStyle(Slider::LinearBar);
    positionSlider->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    
    addAndMakeVisible(randPosLabel);
    randPosLabel.setText("rP", dontSendNotification);
    
    addAndMakeVisible(randPosSlider = new ParameterSlider (*p.randPosParam));
    randPosSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    randPosSlider->setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
    
    // Duration
    addAndMakeVisible(durationLabel);
    durationLabel.setText("Duration", dontSendNotification);
    
    addAndMakeVisible(durationSlider = new ParameterSlider (*p.durationParam));
    durationSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    durationSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);

    
    addAndMakeVisible(randDurSlider = new ParameterSlider(*p.randDurParam));
    randDurSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    randDurSlider->setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
    
    // Density
    addAndMakeVisible(densityLabel);
    densityLabel.setText("Density", dontSendNotification);

    addAndMakeVisible(densitySlider = new ParameterSlider (*p.densityParam));
    densitySlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    densitySlider->setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    
    addAndMakeVisible(randDenSlider = new ParameterSlider (*p.randDensityParam));
    randDenSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    randDenSlider->setTextBoxStyle(Slider::NoTextBox, false, 80, 20);

    // Transposition
    addAndMakeVisible(transLabel);
    transLabel.setText("Transpose", dontSendNotification);
    
    addAndMakeVisible(transSlider = new ParameterSlider (*p.transParam));
    transSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    transSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    
    addAndMakeVisible(randTransSlider = new ParameterSlider (*p.randTransParam));
    randTransSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    randTransSlider->setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
    
    // Volume
    addAndMakeVisible(volLabel);
    volLabel.setText("Volume", dontSendNotification);
    
    addAndMakeVisible(volSlider = new ParameterSlider (*p.volumeParam));
    volSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    volSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    
    addAndMakeVisible(randVolSlider = new ParameterSlider (*p.randVolumeParam));
    randVolSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    randVolSlider->setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
    
    // Envelope
    addAndMakeVisible(envLabel);
    envLabel.setText("Envelope", dontSendNotification);
    
    addAndMakeVisible(envMidLabel);
    envMidLabel.setText("Mid", dontSendNotification);
    
    addAndMakeVisible(envMidSlider = new ParameterSlider (*p.envMidParam));
    envMidSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    envMidSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    
    addAndMakeVisible(envSusLabel);
    envSusLabel.setText("Sustain", dontSendNotification);
    
    addAndMakeVisible(envSusSlider = new ParameterSlider (*p.envSustainParam));
    envSusSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    envSusSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    
    addAndMakeVisible(envCurveLabel);
    envCurveLabel.setText("Curve", dontSendNotification);
    
    addAndMakeVisible(envCurveSlider = new ParameterSlider (*p.envCurveParam));
    envCurveSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    envCurveSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    
    // Thread
    formatManager.registerBasicFormats();
    startThread();
    
    setSize (500, 300);
    
    LOG("PluginEditor constructor called");
}

GrrnlrrAudioProcessorEditor::~GrrnlrrAudioProcessorEditor()
{
    stopThread(4000);
}

//==============================================================================
void GrrnlrrAudioProcessorEditor::resized()
{
    const int width = getWidth();
    
    openButton.setBounds(10, 10, 120, 20);
    
    // Position:
    positionSlider->setBounds(10, 40, width - 50, 20);
    
    randPosLabel.setBounds(width - 30, 20, 25, 20);
    randPosSlider->setBounds(width - 30, 37, 25, 25);
    
    // Duration:
    durationLabel.setBounds(10, 70, 50, 20);
    durationSlider->setBounds(10, 90, 50, 65);
    
    randDurSlider->setBounds(15, 160, 40, 40);
    
    // Density:
    densityLabel.setBounds(70, 70, 70, 20);
    densitySlider->setBounds(70, 90, 50, 65);
    
    randDenSlider->setBounds(75, 160, 40, 40);
    
    // Transposition
    transLabel.setBounds(130, 70, 70, 20);
    transSlider->setBounds(130, 90, 50, 65);
    
    randTransSlider->setBounds(135, 160, 40, 40);
    
    // Volume
    volLabel.setBounds(190, 70, 70, 20);
    volSlider->setBounds(190, 90, 50, 65);
    
    randVolSlider->setBounds(195, 160, 40, 40);
    
    // Envelope
    envLabel.setBounds(320, 70, 70, 20);
    
    envMidLabel.setBounds(320, 85, 70, 20);
    envMidSlider->setBounds(320, 105, 50, 65);
    
    envSusLabel.setBounds(380, 85, 70, 20);
    envSusSlider->setBounds(380, 105, 50, 65);
    
    envCurveLabel.setBounds(440, 85, 70, 20);
    envCurveSlider->setBounds(440, 105, 50, 65);
}

void GrrnlrrAudioProcessorEditor::checkForBuffersToFree()
{
    
}

void GrrnlrrAudioProcessorEditor::checkForRestoredPath()
{
    String path;
    path = processor.restoredPath;
    
    if(path.isNotEmpty()){
        //processor.filePath = path;
        
        swapVariables(chosenPath, path);
        processor.restoredPath = "";
    }
}

void GrrnlrrAudioProcessorEditor::checkForPathToOpen()
{
    String pathToOpen;
    swapVariables(pathToOpen, chosenPath);
    
    if(pathToOpen.isNotEmpty()){
        processor.filePath = pathToOpen;
        loadAudioFile(pathToOpen);
    }
}

void GrrnlrrAudioProcessorEditor::run()
{
    while(! threadShouldExit()){
        checkForRestoredPath();
        checkForPathToOpen();
        checkForBuffersToFree();
        wait(500);
    }
}

void GrrnlrrAudioProcessorEditor::buttonClicked(Button* button)
{
    if(button == &openButton) openButtonClicked();
}

void GrrnlrrAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    
}

void GrrnlrrAudioProcessorEditor::openButtonClicked()
{
    FileChooser chooser ("Select a File to open...",
                         File::nonexistent,
                         "*.wav", "*.aif", "*.aiff");
    
    if(chooser.browseForFileToOpen()){
        const File file (chooser.getResult());
        String path (file.getFullPathName());
        swapVariables (chosenPath, path);
        
        notify();
    }
}

void GrrnlrrAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void GrrnlrrAudioProcessorEditor::loadAudioFile(String path)
{
    const File file (path);
    LOG("Trying to load a file at: " << path << "\n");
    if(file.exists()){
        LOG("We have a file at: " << path << "\n");
        // we create the right kind of AudioFormatReader for our File
        ScopedPointer<AudioFormatReader> reader(formatManager.createReaderFor(file));
        ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer(file.getFileName(),
                                                                           reader->numChannels,
                                                                           reader->lengthInSamples);
        
        if(reader != nullptr){
            // stream the contents of the Audio File into the Buffer
            // args: AudioSampleBuffer*, startSample, endSample, readerStartSample, useLeftChan, useRightChan
            reader->read (newBuffer->getAudioSampleBuffer(), 0, reader->lengthInSamples, 0, true, true);
            std::cout << "Samples in Buffer: " << newBuffer->getAudioSampleBuffer()->getNumSamples() << std::endl;
            
            processor.fileBuffer = newBuffer;
        }
    } else {
        LOG("Sorry but the file you are trying to load does not exist :(");
    }
}
