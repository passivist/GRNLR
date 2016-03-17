/*
 ==============================================================================
 GRNLR VST IMPLEMENTATION
 Concept:
 > quasisyncronous granular synthesis engine based on a supercollider prototype
 > support for an approach to granular synthesis inspired by Curtis Roads book
 "microsound"
 
 !! TODO !!
 STUCTURE:
    > a grainsynth class that handles rendering the block of a single grain
 ENGINE:
    > allow for grains to be shorter than buffersize
    > allow for grains to start and stop inside a running buffer
    > grains have start / end
    > an asynchronous massaging system between the process function and things
      like grain prosition in envelope etc.
 SCHEDULER:
    > should the scheduler run on a different thread than the process application?
    > functionality should be something like SuperCollider's pattern system:
      > scheduling is based on inter-onset times, there is only ever one event
        scheduled at a time
        for example:
	the first event is scheduled 2 seconds into the future
	at the time when this event is scheduled the scheduler is woken up
	and schedules the next event into the future
      > the scheduling is done via creating and destroying tuples of Grain-Objects
        and times on the GrainStack

 !! ISSUES !!
 Plugin should check if everything is initialized and a sample is loaded before
 running the process functions

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
  position = 0;
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
void Grnlr_kleinAudioProcessor::schedule(int startPosition, int length, int time)
{
  
  while(stack.size() > 1 && stack.front().hasEnded)
    {
      stack.pop_front();
    }
  
  stack.push_back(Grain(startPosition, length));
  
  std::cout << "NumGrains: " << stack.size() << std::endl;
  wait(time);
}

void Grnlr_kleinAudioProcessor::run()
{
  while (! threadShouldExit())
    {
      // only schedule something when there is data in the buffer
      if(fileBuffer.getNumSamples() > 0)
	{
	  schedule(positionOffset * fileBuffer.getNumSamples(), lengthRatio * (durationSeconds * sampleRate), durationSeconds * 1000);
	}
    }
}

//==============================================================================
void Grnlr_kleinAudioProcessor::applyEnvelope (AudioSampleBuffer& buffer)
{
  // simple envelope over one loopcycle, replace this with a more sofisticated approach later on
  for (int channel=0; channel<buffer.getNumChannels(); ++channel)
    {
      buffer.applyGainRamp(channel, 0, buffer.getNumSamples()/2, 0, 1 );
      buffer.applyGainRamp(channel, buffer.getNumSamples()/2, buffer.getNumSamples(), 1, 0);
    }
}

void Grnlr_kleinAudioProcessor::addBuffers(AudioSampleBuffer& bufferA, AudioSampleBuffer& bufferB)
{
  const int numSamples = bufferA.getNumSamples();
  for (int channel = 0; channel < bufferA.getNumChannels(); ++channel)
    {
      float* const channelDataA = bufferA.getWritePointer (channel);
      float* const channelDataB = bufferB.getWritePointer (channel);
      
      for (int i = 0; i < numSamples; ++i)
        {
	  channelDataA[i] += channelDataB[i];
        }
    }
}

void Grnlr_kleinAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
  buffer.clear();
  // test if the is actually data in the buffer
  if(fileBuffer.getNumSamples() > 0 && stack.size() > 0 ) {
    for(int i=0; i<stack.size(); ++i)
      {
	tempBuffer = buffer;
	stack[i].process(buffer, fileBuffer);
	addBuffers(tempBuffer, buffer);
      }
  }
  // applyEnvelope(buffer);
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
