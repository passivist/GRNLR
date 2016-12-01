/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Grain.h"
#include "ReferenceCountedBuffer.h"
#include "ParameterGUIclasses.h"

//==============================================================================
/**
*/
class GrrnlrrAudioProcessor  : public AudioProcessor,
                               public Thread
{
public:
    //==============================================================================
    GrrnlrrAudioProcessor();
    ~GrrnlrrAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet);
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;
    void processMidi (MidiBuffer& midiMessages, int numSamples);

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; };

    //==============================================================================
    const String getName() const override { return JucePlugin_Name; };

    bool acceptsMidi() const override { return true; };
    bool producesMidi() const override { return false; };
    double getTailLengthSeconds() const override { return 0.0; };

    //==============================================================================
    int getNumPrograms() override { return 1; };
    int getCurrentProgram() override { return 0; };
    void setCurrentProgram (int index) override { };
    const String getProgramName (int index) override { return String(); } ;
    void changeProgramName (int index, const String& newName) override { };

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void run() override;
    
    double sampleRate;
    long long int time;
    long long int nextGrainOnset;
    
    Array<Grain> grainStack;
    
    int midiNotes[128] = {0};
    
    ReferenceCountedBuffer::Ptr fileBuffer;
    
    // Sample Path
    String filePath;
    String restoredPath;

    // Utility
    int wrap(int val, const int lo, const int hi);
    float clip(float n, float lower, float upper);
    
    
    
    // Parameters
    // Main Grain Parameters:
    AudioParameterFloat* positionParam;
    AudioParameterFloat* randPosParam;
    AudioParameterFloat* directionParam;
    AudioParameterFloat* densityParam;
    AudioParameterFloat* randDensityParam;
    AudioParameterFloat* durationParam;
    AudioParameterFloat* randDurParam;
    AudioParameterFloat* transParam;
    AudioParameterFloat* randTransParam;
    AudioParameterFloat* volumeParam;
    AudioParameterFloat* randVolumeParam;
    AudioParameterBool* holdParam;
    
    // Envelope Parameters:
    AudioParameterFloat* envMidParam;
    AudioParameterFloat* envSustainParam;
    AudioParameterFloat* envCurveParam;
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrrnlrrAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
