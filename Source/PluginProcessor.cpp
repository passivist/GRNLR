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
    > an asynchronous massaging system between the process function and things
      like grain prosition in envelope etc.

 !! ISSUES !!
 Klicks between Grains, either something to do with grain handling or envelopes
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
  // maybe this should even be solved so the editor sends reasonable values on startup
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
      if (sampleIsLoaded)
	{
	  schedule(positionOffset * lengthInSamples, lengthRatio * (durationSeconds * sampleRate), durationSeconds * 1000);
	}
      else
	{
	wait(500);
	}
    }
}

//==============================================================================
void Grnlr_kleinAudioProcessor::addBuffers(AudioSampleBuffer& bufferA, AudioSampleBuffer& bufferB)
{
  const int numSamples = jmin(bufferA.getNumSamples(), bufferB.getNumSamples() );

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
  // check if a valid buffer exists
  ReferenceCountedBuffer::Ptr retainedCurrentBuffer (currentBuffer);
  if (retainedCurrentBuffer == nullptr)
    {
      buffer.clear();
      return;
    }

  AudioSampleBuffer* currentAudioSampleBuffer (retainedCurrentBuffer->getAudioSampleBuffer());

  if(stack.size() > 0 )
    {
      for(int i=0; i<stack.size(); ++i)
	{
	  tempBuffer = buffer;
	  stack[i].process(buffer, *currentAudioSampleBuffer);
	  addBuffers(tempBuffer, buffer);
	}
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
