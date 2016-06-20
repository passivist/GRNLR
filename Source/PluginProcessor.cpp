/*
 ==============================================================================
 GRNLR VST IMPLEMENTATION
 Concept:
 > quasisyncronous granular synthesis engine based on a supercollider prototype
 > support for an approach to granular synthesis inspired by Curtis Roads book
 "microsound"
 
 !! TODO !!
 STUCTURE:
 ENGINE:
 > A variable Envelope
   > Env Curve
 > high level randomisation of grain Events
 
 > radomisation but also algorithmic creation of grain streams
 
 > an asynchronous massaging system between the process function and things
 
 > reverse grains
 
 > transpose grains
 
 > host automation
 
 !! ISSUES !!
 > Scheduler sometimes sends bad values for first grain:
 JUCE Assertion failure in juce_AudioSampleBuffer.h:641
 JUCE Assertion failure in juce_AudioSampleBuffer.h:641
 SCHEDULER:Startposition: 57768.3 length: 66991.6 dt: 500
 > resulting in a grain that doesn't end due to bad values:
 STACK: Ended: 0 Startposition: -2147483648 current position: -2147039007 length: 66991
 
 > Envelope behaves strange due to bad offsetting
 > investigate further
 
 > offsets are negative for small duration values
 
 
 > There are Clicks sometimes
 
 > program crashes on sample loading sometimes
 
 
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Grnlr_kleinAudioProcessor::Grnlr_kleinAudioProcessor() : Thread("BackgroundThread")
{
    startThread();
    schedulerLatency = 882;
}

Grnlr_kleinAudioProcessor::~Grnlr_kleinAudioProcessor()
{
    stopThread(4000);
}

//==============================================================================
const String Grnlr_kleinAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Grnlr_kleinAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Grnlr_kleinAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool Grnlr_kleinAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double Grnlr_kleinAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Grnlr_kleinAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int Grnlr_kleinAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Grnlr_kleinAudioProcessor::setCurrentProgram (int index)
{
    
}

const String Grnlr_kleinAudioProcessor::getProgramName (int index)
{
    return String();
}

void Grnlr_kleinAudioProcessor::changeProgramName (int index, const String& newName)
{
    
}

//==============================================================================
void Grnlr_kleinAudioProcessor::prepareToPlay (double sRate, int samplesPerBlock)
{
    time = 0;
    sampleRate = sRate;
}

void Grnlr_kleinAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

//==============================================================================
void Grnlr_kleinAudioProcessor::schedule(int startPosition, int length, float dur, float center, float sustain, float curve)
{
    int onset = (dur*sampleRate) + time + schedulerLatency;

    std::cout << "NumGrains: " << stack.size() << std::endl;
    
    stack.push_back(Grain(startPosition, length, onset, center, sustain, curve));
    
    wait(dur*1000);
}

void Grnlr_kleinAudioProcessor::run()
{
    while (! threadShouldExit())
    {
        if (sampleIsLoaded)
        {
            int grainLength = lengthRatio * (durationSeconds * sampleRate);
            if (grainLength < 1) grainLength = 1;   // for safety if by some combination of parameters the length is 0
            schedule( positionOffset * lengthInSamples,                 // startPosition
                     grainLength,                                       // length
                     durationSeconds,                                   // duration
                     envCenter,                                         // center
                     envSustain,                                        // sustain
                     1 );                                               // curve
        }
        else
        {
            wait(500);
        }
    }
}

//==============================================================================
void Grnlr_kleinAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    
    int blockSize = buffer.getNumSamples();
    
    
    // before we do anything we clear the current buffer to avoid noise:
    buffer.clear();
    
    // check if a valid buffer exists
    ReferenceCountedBuffer::Ptr retainedCurrentBuffer (currentBuffer);
    if (retainedCurrentBuffer == nullptr)
    {
        return;
    }
    
    AudioSampleBuffer* currentAudioSampleBuffer (retainedCurrentBuffer->getAudioSampleBuffer());
    int numChannels = currentAudioSampleBuffer->getNumChannels();
    
    for (int i=0; i < blockSize; ++i) {
        for(int i=0; i<stack.size(); ++i)
        {
            if(time > stack[i].onset + stack[i].grainLength)
            {
                stack.erase(stack.begin() + i);
            }
            
            if(time - stack[i].onset > 0){
                stack[i].process(buffer, *currentAudioSampleBuffer, time, numChannels, blockSize);
            }
        }
        
        time += 1;
    }
}

//==============================================================================
bool Grnlr_kleinAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Grnlr_kleinAudioProcessor::createEditor()
{
    return new Grnlr_kleinAudioProcessorEditor (*this);
}

//==============================================================================
void Grnlr_kleinAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Grnlr_kleinAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Grnlr_kleinAudioProcessor();
}
