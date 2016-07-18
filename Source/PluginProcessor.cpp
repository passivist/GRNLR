/*
 ==============================================================================
 GRNLR VST IMPLEMENTATION
 Concept:
 > quasisyncronous granular synthesis engine based on a supercollider prototype
 > support for an approach to granular synthesis inspired by Curtis Roads book
 "microsound"
 
 !! TODO !!
 > A variable Envelope
 > Env Curve
 > maybe have Envelope be rendered during grain creation?? -> maybe more efficient, but no
 problems due to the envelope calculation being to costly thus far
 
 > reverse grains
 > have a flag in the grain object which just tells the processfunction to read the samples
 in the reverse order
 
 
 > implement MIDI Transposition
 > implement MIDI Gate
 
 !! ISSUES !!
 > Audioparameters have only linear mapping so far should be exponetional in some cases
 
 > handle samplerate mismatch between loaded file and host application
 
 
 
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Grnlr_kleinAudioProcessor::Grnlr_kleinAudioProcessor() : Thread("BackgroundThread"),
                                                         positionParam(nullptr),
                                                         randPosParam(nullptr),
                                                         fillFactorParam(nullptr),
                                                         randFillParam(nullptr),
                                                         durationParam(nullptr),
                                                         randDurParam(nullptr),
                                                         transParam(nullptr),
                                                         randTransParam(nullptr),
                                                         holdParam(nullptr),
                                                         envCenterParam(nullptr),
                                                         envSustainParam(nullptr),
                                                         volumeParam(nullptr),
                                                         randVolumeParam(nullptr)

{
    startThread();
    schedulerLatency = 882;
    
    addParameter(positionParam = new AudioParameterFloat("pos", "Position", 0.0f, 1.0f, 0.5f));
    addParameter(randPosParam = new AudioParameterFloat("randPos", "Random Position", 0.0f, 1.0f, 0.0f));
    addParameter(fillFactorParam = new AudioParameterFloat("fill", "Fill Factor", 0.01f, 32.0f, 4.0f));
    addParameter(randFillParam = new AudioParameterFloat("randFill", "Random Fill Factor", 0.0f, 1.0f, 0.0f));
    addParameter(durationParam = new AudioParameterFloat("dur", "Duration", 0.001f, 2.0f, 0.3f));
    addParameter(randDurParam = new AudioParameterFloat("randDur", "Random Duration", 0.0f, 1.0f, 0.0f));
    addParameter(transParam = new AudioParameterFloat("trans", "Transposition", -48.0f, 48.0f, 0.0f));
    addParameter(randTransParam = new AudioParameterFloat("randTrans", "Random Trans", 0.0f, 1.0f, 0.0f ));
    addParameter(envCenterParam = new AudioParameterFloat("envCenter", "Envelope Center", 0.0f, 1.0f, 0.5f));
    addParameter(envSustainParam = new AudioParameterFloat("envSustain", "Envelope Sustain", 0.0f, 1.0f, 0.5f));
    addParameter(volumeParam = new AudioParameterFloat("vol", "Volume", 0.0f, 1.0f, 0.7f));
    addParameter(randVolumeParam = new AudioParameterFloat("randVol", "Random Volume", 0.0f, 1.0f, 0.0f));
    
    addParameter(holdParam = new AudioParameterBool("hold", "Hold", false));
    
    random = *new Random(Time::currentTimeMillis());
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
    
    keyboardState.reset();
}

void Grnlr_kleinAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

//==============================================================================
void Grnlr_kleinAudioProcessor::schedule(int startPosition, int length, float dur, float trans, float center, float sustain, float curve)
{
    int onset = (dur*sampleRate) + time + schedulerLatency;
    
    //std::cout << "NumGrains: " << stack.size() << std::endl;
    
    stack.push_back(Grain(startPosition, length, onset, trans, center, sustain, curve));
    
    wait(dur*1000);
}

void Grnlr_kleinAudioProcessor::run()
{
    while (! threadShouldExit())
    {
        if (sampleIsLoaded) {
            // Get Active Notes
            std::vector<std::vector<int>> activeNotes;
            
            for(int i=0; i<128; i++){
                if(midiNotes[i] > 0){
                    activeNotes.push_back( std::vector<int> {i, midiNotes[i] } );
                }
            }
            
            if(*holdParam || (activeNotes.size()>0)) {
                float midiNote = 60;
                
                if(activeNotes.size()>0){
                    midiNote = activeNotes[random.nextInt(activeNotes.size())][0];
                }
                
                midiNote = (midiNote - 60);
                
                float position   = *positionParam   + (*randPosParam * (random.nextFloat() - 0.5));
                float duration   = *durationParam   * (1 + (*randDurParam * (random.nextFloat() * 2 - 1)));
                float fillFactor = *fillFactorParam * (1 + (*randFillParam * (random.nextFloat() * 2 - 1)));
                float trans      = (midiNote + *transParam + 1)  * (1 + (*randTransParam * (random.nextFloat() * 2 - 1))) - 1;
                float envCenter  = *envCenterParam;
                float envSustain = *envSustainParam;
                
                int grainLength = fillFactor * (duration * sampleRate);
                if (grainLength < 1) grainLength = 1;   // for safety if by some combination of parameters the length is 0
                
                schedule( position * lengthInSamples,                       // startPosition
                         grainLength,                                       // length
                         duration,                                          // duration
                         trans,                                             // transposition
                         envCenter,                                         // center
                         envSustain,                                        // sustain
                         1 );                                               // curve
            }
        } else {
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
    
    processMidi(midiMessages, blockSize);
    
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
void Grnlr_kleinAudioProcessor::processMidi(MidiBuffer& midiMessages, int numSamples)
{
    MidiBuffer::Iterator i (midiMessages);
    MidiMessage message;
    int time;
    
    while(i.getNextEvent(message, time))
    {
        if(message.isNoteOn()) {
            midiNotes[message.getNoteNumber()] = message.getVelocity();
        }
        if(message.isNoteOff()) {
            midiNotes[message.getNoteNumber()] = 0;
        }
        if(message.isAllNotesOff()) {
            for(int i=0; i < 128; i++)
                midiNotes[i] = 0;
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
    // Here's an example of how you can use XML to make it easy and more robust:
    std::cout << "Save Settings" << std::endl;
    
    // Create an outer XML element..
    XmlElement xml ("GRNLRPLUGINSETTINGS");
    
    // Store the values of all our parameters, using their param ID as the XML attribute
    for (int i = 0; i < getNumParameters(); ++i)
    {
        if (AudioProcessorParameterWithID* p = dynamic_cast<AudioProcessorParameterWithID*> (getParameters().getUnchecked(i)))
        {
            xml.setAttribute (p->paramID, p->getValue());
            std::cout << p->paramID << " " << p->getValue() << std::endl;
        }
    }
    
    xml.setAttribute("FilePath", filePath);
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
}

void Grnlr_kleinAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::cout << "Load Settings" << std::endl;
    
    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if (xmlState != nullptr)
    {
        // make sure that it's actually our type of XML object..
        if (xmlState->hasTagName ("GRNLRPLUGINSETTINGS"))
        {
            // Now reload our parameters..
            for (int i = 0; i < getNumParameters(); ++i)
            {
                if (AudioProcessorParameterWithID* p = dynamic_cast<AudioProcessorParameterWithID*> (getParameters().getUnchecked(i)))
                {
                    p->setValueNotifyingHost ((float) xmlState->getDoubleAttribute (p->paramID, p->getValue()));
                    std::cout << p->paramID << " " << p->getValue() << std::endl;
                }
            }
            loadedPath = xmlState->getStringAttribute("FilePath");
            std::cout << loadedPath << std::endl;
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Grnlr_kleinAudioProcessor();
}
