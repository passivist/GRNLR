/*
 ==============================================================================

 This file was auto-generated by the Introjucer!

 It contains the basic framework code for a JUCE plugin processor.

 ==============================================================================
 */

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Grain.h"
#include <vector>



//==============================================================================
/**
 */
class ReferenceCountedBuffer : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<ReferenceCountedBuffer> Ptr;

    ReferenceCountedBuffer (const String& nameToUse,
                            int numChannels,
                            int numSamples) : position (0),
    name (nameToUse),
    buffer (numChannels, numSamples)
    {
        DBG (
             String ("Buffer named '") + name +
             "' constructed. numChannels = " + String (numChannels) +
             ", numSamples = " + String (numSamples) );
    }

    ~ReferenceCountedBuffer()
    {
        DBG (String ("Buffer named '") + name + "' destroyed");
    }

    AudioSampleBuffer* getAudioSampleBuffer()
    {
        return &buffer;
    }

    int position;

private:
    String name;
    AudioSampleBuffer buffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReferenceCountedBuffer)
};

class Grnlr_kleinAudioProcessor  : public AudioProcessor,
public Thread
{
public:
    //==============================================================================
    Grnlr_kleinAudioProcessor();
    ~Grnlr_kleinAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    void run () override;
    //==============================================================================
    void schedule(int startPosition, int length, float durSecs, float center, float sustain, float curve);

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool silenceInProducesSilenceOut() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    AudioSampleBuffer tempBuffer;
    std::vector<Grain> stack;

    ReferenceCountedBuffer::Ptr currentBuffer;

    // CONTROLS
    // Main Grain Controls:
    AudioParameterFloat* positionParam;
    AudioParameterFloat* randPosParam;
    AudioParameterFloat* fillFactorParam;
    AudioParameterFloat* randFillParam;
    AudioParameterFloat* durationParam;
    AudioParameterFloat* randDurParam;
    AudioParameterFloat* transParam;
    AudioParameterFloat* randTransParam;

    // Envelope:
    AudioParameterFloat* envCenterParam;
    AudioParameterFloat* envSustainParam;

    bool sampleIsLoaded = false;
    String filePath;
    String loadedPath;

    int lengthInSamples;
    int positionOffsetInSamples;
    int durationMillis;

    int schedulerLatency;

    long long int time; // maybe I'll have to find a better way to
                        // represent time than by samples... Leaving
                        // this program running for more than 1 Day
                        // treads into 64 bit territory. Although this
                        // still runs on 32bit machines I feel there
                        // has to be a better way...

    int sampleRate;

private:
    Random random;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Grnlr_kleinAudioProcessor)
};

#endif  // PLUGINPROCESSOR_H_INCLUDED
