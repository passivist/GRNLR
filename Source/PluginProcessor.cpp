/*
  ==============================================================================
  GRNLR VST IMPLEMENTATION
  Concept:
  > quasisyncronous granular synthesis engine based on a supercollider prototype
  > support for an approach to granular synthesis inspired by Curtis Roads book
    "microsound"
  
  !! TODO !!
  STUCTURE:
    > class for handling grain scheduling 
    > a grainsynth class that handles rendering the block of a single grain
  ENGINE:
    > allow for grains to be shorter than buffersize
    > allow for grains to start and stop inside a running buffer
    > grains have start / end
    > an asynchronous massaging system between the process function and things
      like grain prosition in envelope etc.
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
Grnlr_kleinAudioProcessor::Grnlr_kleinAudioProcessor()
{
}

Grnlr_kleinAudioProcessor::~Grnlr_kleinAudioProcessor()
{
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
void Grnlr_kleinAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
  position = 0;
  lengthRatio = 0;
  updateValues();
}

void Grnlr_kleinAudioProcessor::releaseResources()
{
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

void Grnlr_kleinAudioProcessor::applyEnvelope (AudioSampleBuffer& buffer)
{
  for (int channel=0; channel<buffer.getNumChannels(); ++channel)
    {
      buffer.applyGainRamp(channel, 0, buffer.getNumSamples()/2, 0, 1 );
      buffer.applyGainRamp(channel, buffer.getNumSamples()/2, buffer.getNumSamples(), 1, 0);
    }
}

void Grnlr_kleinAudioProcessor::updateValues ()
{
  lengthInSamples         = lengthRatio * fileBuffer.getNumSamples();
  positionOffsetInSamples = positionOffset * fileBuffer.getNumSamples();
}

void Grnlr_kleinAudioProcessor::loadSamples (AudioSampleBuffer& buffer, int startSample, int numSamples)
{
  int outputSamplesRemaining  = buffer.getNumSamples();
  int outputSamplesOffset     = 0;
  
  if(position < numSamples) {
    int bufferSamplesRemaining = fileBuffer.getNumSamples() - position;
    int samplesThisBlock = jmin(outputSamplesRemaining, bufferSamplesRemaining);
    
    for (int channel=0; channel < buffer.getNumChannels(); ++channel) {
      buffer.copyFrom( channel,
		       outputSamplesOffset,
		       fileBuffer,
		       channel % fileBuffer.getNumChannels(),
		       (position + startSample) % fileBuffer.getNumSamples(),
		       samplesThisBlock );
    }
    
    outputSamplesRemaining  -= samplesThisBlock;
    outputSamplesOffset     += samplesThisBlock;
    position                += samplesThisBlock;
  } else {
    outputSamplesRemaining = buffer.getNumSamples();
    position = 0;
    updateValues();
  }
}

void Grnlr_kleinAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{ 
  loadSamples(buffer, positionOffsetInSamples, lengthInSamples);
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
