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
 
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformView.h"
#include "PluginProcessor.h"



//==============================================================================
/**
*/
class GrrnlrrAudioProcessorEditor  : public AudioProcessorEditor,
                                     private Button::Listener,
                                     private Slider::Listener,
                                     private ChangeListener,
                                     private Timer
{
public:
    GrrnlrrAudioProcessorEditor (GrrnlrrAudioProcessor&);
    ~GrrnlrrAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void buttonClicked(Button* button) override;
    void sliderValueChanged(Slider* slider) override;
    void changeListenerCallback (ChangeBroadcaster* source) override;
    
    void timerCallback () override;
    
    void openButtonClicked();
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GrrnlrrAudioProcessor& processor;
    
    // GUI
    TextButton openButton;
    ScopedPointer<WaveformView> waveform;

    // Position:
    ScopedPointer<ParameterSlider> positionSlider;
    
    Label randPosLabel;
    ScopedPointer<ParameterSlider> randPosSlider;

	Label spreadLabel;
	ScopedPointer<ParameterSlider> spreadSlider;
    
    //  Duration
    Label durationLabel;
    ScopedPointer<ParameterSlider> durationSlider;
    ScopedPointer<ParameterSlider> randDurSlider;
	
    
    // Density:
    Label densityLabel;
    ScopedPointer<ParameterSlider> densitySlider;
    ScopedPointer<ParameterSlider> randDenSlider;
    
    // Transposition:
    Label transLabel;
    ScopedPointer<ParameterSlider> transSlider;
    ScopedPointer<ParameterSlider> randTransSlider;
    
    // Volume:
    Label volLabel;
    ScopedPointer<ParameterSlider> volSlider;
    ScopedPointer<ParameterSlider> randVolSlider;
    
    // Envelope:
    Label envLabel;
    Label envMidLabel;
    ScopedPointer<ParameterSlider> envMidSlider;
    
    Label envSusLabel;
    ScopedPointer<ParameterSlider> envSusSlider;
    
    Label envCurveLabel;
    ScopedPointer<ParameterSlider> envCurveSlider;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrrnlrrAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
