/*
 GRNLR - a granular synthesis instrument
 Copyright (C) 2017  Raffael Seyfried
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
GrrnlrrAudioProcessorEditor::GrrnlrrAudioProcessorEditor (GrrnlrrAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    addAndMakeVisible(waveform = new WaveformView (p.formatManager));
    waveform->addChangeListener (this);
    
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
    
    startTimerHz(10);
    
    setSize (500, 320);
    
    LOG("PluginEditor constructor called");
}

GrrnlrrAudioProcessorEditor::~GrrnlrrAudioProcessorEditor()
{
    
}

//==============================================================================
void GrrnlrrAudioProcessorEditor::resized()
{
    const int width = getWidth();
    
    // Waveform
    waveform->setBounds(10, 5, 480, 100);
    
    if(processor.filePath.isNotEmpty()){
        const File file (processor.filePath);
        waveform->setFile(file);
    }
    
    openButton.setBounds(10, 115, 120, 20);
    
    // Position:
    positionSlider->setBounds(10, 140, width - 50, 20);
    
    randPosLabel.setBounds(width - 30, 120, 25, 20);
    randPosSlider->setBounds(width - 30, 137, 25, 25);
    
    // Duration:
    durationLabel.setBounds(10, 170, 50, 20);
    durationSlider->setBounds(10, 190, 50, 65);
    
    randDurSlider->setBounds(15, 260, 40, 40);
    
    // Density:
    densityLabel.setBounds(70, 170, 70, 20);
    densitySlider->setBounds(70, 190, 50, 65);
    
    randDenSlider->setBounds(75, 260, 40, 40);
    
    // Transposition
    transLabel.setBounds(130, 170, 70, 20);
    transSlider->setBounds(130, 190, 50, 65);
    
    randTransSlider->setBounds(135, 260, 40, 40);
    
    // Volume
    volLabel.setBounds(190, 170, 70, 20);
    volSlider->setBounds(190, 190, 50, 65);
    
    randVolSlider->setBounds(195, 260, 40, 40);
    
    // Envelope
    envLabel.setBounds(320, 170, 70, 20);
    
    envMidLabel.setBounds(320, 185, 70, 20);
    envMidSlider->setBounds(320, 205, 50, 65);
    
    envSusLabel.setBounds(380, 185, 70, 20);
    envSusSlider->setBounds(380, 205, 50, 65);
    
    envCurveLabel.setBounds(440, 185, 70, 20);
    envCurveSlider->setBounds(440, 205, 50, 65);
}

void GrrnlrrAudioProcessorEditor::timerCallback()
{
    if(processor.filePath.isNotEmpty()){
        const File file (processor.filePath);
        waveform->setFile(file);
    }
}

void GrrnlrrAudioProcessorEditor::buttonClicked(Button* button)
{
    if(button == &openButton) openButtonClicked();
}

void GrrnlrrAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    
}

// This function implements a callback that the WaveformView class sends when sendChangeMessage() is
// called in the files dropped function. We use it to load the file that we have dropped in the
// WaveformView
void GrrnlrrAudioProcessorEditor::changeListenerCallback (ChangeBroadcaster* source)
{
    if (source == waveform){
        const File file(waveform->getLastDroppedFile());
        String path (file.getFullPathName());
        swapVariables(processor.chosenPath, path);
        waveform->setFile(file);
    }
}

void GrrnlrrAudioProcessorEditor::openButtonClicked()
{
    FileChooser chooser ("Select a File to open...",
                         File::nonexistent,
                         "*.wav", "*.aif", "*.aiff");
    
    if(chooser.browseForFileToOpen()){
        const File file (chooser.getResult());
        String path (file.getFullPathName());
        waveform->setFile(file);
        swapVariables (processor.chosenPath, path);
    }
}

void GrrnlrrAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}
