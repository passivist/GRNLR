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
 > allow for grains to start and stop inside a running buffer with offsets
   > for this we need to schedule into the future instead of right away
 > rewrite scheduling so we schedule with [time, Grain] into the future
   the audio process should then check if and where in the block the grain
   starts and adjust things accordingly, for this we also need outputSampleOffset  
 > an asynchronous massaging system between the process function and things
 > reverse grains
 > transpose grains
 
 !! ISSUES !!
 > Scheduler sometimes sends bad values for first grain:
   JUCE Assertion failure in juce_AudioSampleBuffer.h:641
   JUCE Assertion failure in juce_AudioSampleBuffer.h:641
   SCHEDULER:Startposition: 57768.3 length: 66991.6 dt: 500
 > resulting in a grain that doesn't end due to bad values:
   STACK: Ended: 0 Startposition: -2147483648 current position: -2147039007 length: 66991
 
 > There is a Click every block
 
 > program crashes on sample loading

 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Grnlr_kleinAudioProcessor::Grnlr_kleinAudioProcessor() : Thread("BackgroundThread")
{
    startThread();
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
    // initialize some values, maybe some of this stuff belongs in the constructor (?)
    // maybe this should even be solved so the editor always sends and
    // displays reasonable values on startup
    time = 0;
    lengthRatio = 0.3;
    durationSeconds = 0.5;
    sampleRate = sRate;
}

void Grnlr_kleinAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

//==============================================================================
void Grnlr_kleinAudioProcessor::schedule(int startPosition, int length, float dur)
{ 
  if(stack.size() > 0 )
    {
        // never modify a collection you are iterating over!!!
        // find safer way to do this!
        for(int i=0; i<stack.size(); ++i)
        {
            // show some information about the grains on the stack
            // std::cout   << "STACK: Ended: " << stack[i].hasEnded
            //             << " Startposition: " << stack[i].startPosition
            //             << " current position: " << stack[i].currentPosition
            //             << " length: " << stack[i].grainLength
            //             << " Start time: " << stack[i].startTime
            //             << " time now: " << time
            //             << std::endl;
             
            
            if (stack[i].hasEnded) {
                stack.erase(stack.begin() + i);
            }
        }
    }
  float startTime = (dur*sampleRate) + time;
  
  stack.push_back(Grain(startPosition, length, startTime));
    
  // std::cout << "NumGrains: " << stack.size() << "\n" << std::endl;
  wait(dur*1000);
}

void Grnlr_kleinAudioProcessor::run()
{
    while (! threadShouldExit())
    {
        if (sampleIsLoaded)
        {
            schedule(positionOffset * lengthInSamples, lengthRatio * (durationSeconds * sampleRate), durationSeconds);
                // show some information about the grains on the stack
                // std::cout   << "SCHEDULER:"
                //            << "Startposition: " << (positionOffset * lengthInSamples)
                //            << " length: " << (lengthRatio * (durationSeconds * sampleRate))
                //            << " dt: " << (durationSeconds * 1000)
                //            << std::endl;
            
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

    //std::cout << "time: " << time << std::endl;
    int blockSize = buffer.getNumSamples();
    int offset;
    
    // check if a valid buffer exists
    ReferenceCountedBuffer::Ptr retainedCurrentBuffer (currentBuffer);
    if (retainedCurrentBuffer == nullptr)
    {
        buffer.clear();
        return;
    }
    
    AudioSampleBuffer* currentAudioSampleBuffer (retainedCurrentBuffer->getAudioSampleBuffer());
    
    if(stack.size() > 0)
    {
        for(int i=0; i<stack.size(); ++i)
        {
            tempBuffer = buffer;
            // check if startTime is not too far (more than one block)
            // in the future. Maybe think about edge cases some more
            // here
            // implement all 4 cases (at least the first two: grain is
            // starting and grain is playing)
            if(stack[i].startTime < (time + blockSize)){
                // only calculate an offset when the grain is starting
                if(stack[i].currentPosition == 0){
                    offset = stack[i].startTime - time;
                    // offset seems to be negative sometimes, we need
                    // to find the underlying issue here this is just
                    // a safeguard
                    if(offset < 0) offset = 0;
                } else {
                    offset = 0;
                }
                stack[i].process(buffer, *currentAudioSampleBuffer, offset);
            }
        }
    }
    
    // Update time by the number of samples in this Audio-Block:
    time += blockSize;
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
