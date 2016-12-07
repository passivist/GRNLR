/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class GrrnlrrAudioProcessorEditor  : public AudioProcessorEditor,
                                     public Thread,
                                     private Button::Listener,
                                     private Slider::Listener
{
public:
    GrrnlrrAudioProcessorEditor (GrrnlrrAudioProcessor&);
    ~GrrnlrrAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void run() override;
    
    void buttonClicked(Button* button) override;
    void sliderValueChanged(Slider* slider) override;
    
    void openButtonClicked();
    
    void checkForRestoredPath();
    void checkForPathToOpen();
    void checkForBuffersToFree();
    
    void loadAudioFile(String);
    AudioFormatManager formatManager;
    
    String chosenPath;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GrrnlrrAudioProcessor& processor;
    
    // GUI
    TextButton openButton;

    // Position:
    ScopedPointer<ParameterSlider> positionSlider;
    
    Label randPosLabel;
    ScopedPointer<ParameterSlider> randPosSlider;
    
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
