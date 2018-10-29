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

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//==============================================================================
GrrnlrrAudioProcessor::GrrnlrrAudioProcessor() :    Thread("scheduling thread"),
                                                    positionParam(nullptr),
                                                    randPosParam(nullptr),
													spreadParam(nullptr),
                                                    densityParam(nullptr),
                                                    randDensityParam(nullptr),
                                                    durationParam(nullptr),
                                                    randDurParam(nullptr),
                                                    transParam(nullptr),
                                                    randTransParam(nullptr),
                                                    volumeParam(nullptr),
                                                    randVolumeParam(nullptr),
                                                    envMidParam(nullptr),
                                                    envSustainParam(nullptr),
                                                    envCurveParam(nullptr)
{
    addParameter(positionParam      = new AudioParameterFloat("pos"       , "Position"          , 0.0001f, 1.0f, 0.5f));
    addParameter(randPosParam       = new AudioParameterFloat("randPos"   , "Random Position"   , NormalisableRange<float>(0.0, 1.0, 0.01, 0.5), 0.0f));
	addParameter(spreadParam		= new AudioParameterFloat("spread"	  , "Spread"			, NormalisableRange<float>(0.0, 1.0, 0.001, 0.30), 0.0f));
    addParameter(densityParam       = new AudioParameterFloat("den"       , "Density"           , NormalisableRange<float>(0.001, 80.0, 0.001, 0.2), 2.0f));
    addParameter(randDensityParam   = new AudioParameterFloat("randDen"   , "Random Density"    , 0.0f, 1.0f, 0.0f));
    addParameter(durationParam      = new AudioParameterFloat("dur"       , "Duration"          , NormalisableRange<float>(0.001, 4, 0.001, 0.3), 0.3f));
    addParameter(randDurParam       = new AudioParameterFloat("randDur"   , "Random Duration"   , 0.0f, 1.0f, 0.0f));
    addParameter(transParam         = new AudioParameterFloat("trans"     , "Transposition"     , -48.0f, 48.0f, 0.0f));
    addParameter(randTransParam     = new AudioParameterFloat("randTrans" , "Random Trans"      , NormalisableRange<float>(0, 24.0, 0.001, 0.5), 0.0f ));
    addParameter(volumeParam        = new AudioParameterFloat("vol"       , "Volume"            , NormalisableRange<float>(0.001, 1.0, 0.001, 0.7), 0.7f));
    addParameter(randVolumeParam    = new AudioParameterFloat("randVol"   , "Random Volume"     , 0.0f, 1.0f, 0.0f));
    addParameter(envMidParam        = new AudioParameterFloat("envCenter" , "Envelope Center"   , 0.0f, 1.0f, 0.5f));
    addParameter(envSustainParam    = new AudioParameterFloat("envSustain", "Envelope Sustain"  , 0.0f, 1.0f, 0.5f));
    addParameter(envCurveParam      = new AudioParameterFloat("envCurve"  , "Envelope Curve"    , NormalisableRange<float>(-12, 12, 0.01, 1), 0.0f));
    
    Logger::setCurrentLogger(grLog);
    
    time = 0;
    formatManager.registerBasicFormats();
    startThread();
}

GrrnlrrAudioProcessor::~GrrnlrrAudioProcessor()
{
    stopThread(4000);
    
    Logger::setCurrentLogger(nullptr);
    delete grLog;
}

//==============================================================================
void GrrnlrrAudioProcessor::prepareToPlay (double sRate, int samplesPerBlock)
{
    sampleRate = sRate;
}

void GrrnlrrAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GrrnlrrAudioProcessor::setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet)
{
    // Reject any bus arrangements that are not compatible with your plugin

    const int numChannels = preferredSet.size();  

   #if JucePlugin_IsMidiEffect
    if (numChannels != 0)
        return false;
   #elif JucePlugin_IsSynth
    if (isInput || (numChannels != 1 && numChannels != 2))
        return false;
   #else
    if (numChannels != 1 && numChannels != 2)
        return false;

    if (! AudioProcessor::setPreferredBusArrangement (! isInput, bus, preferredSet))
        return false;
   #endif

    return setPreferredBusArrangement (isInput, bus, preferredSet);
}
#endif

//==============================================================================
int GrrnlrrAudioProcessor::wrap(int val, const int low, const int high)
{
    int range_size = high - low + 1;
    
    if (val < low)
        val += range_size * ((low - val) / range_size + 1);
    
    return low + (val - low) % range_size;
}

float GrrnlrrAudioProcessor::clip(float n, float lower, float upper) {
    return std::max(lower, std::min(n, upper));
}

//==============================================================================

void GrrnlrrAudioProcessor::loadAudioFile(String path)
{
    const File file (path);
    LOG("Trying to load a file at: " << path << "\n");
    if(file.exists()){
        LOG("We have a file at: " << path << "\n");
        // we create the right kind of AudioFormatReader for our File
        ScopedPointer<AudioFormatReader> reader(formatManager.createReaderFor(file));
        ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer(file.getFileName(),
                                                                           reader->numChannels,
                                                                           reader->lengthInSamples);
        
        if(reader != nullptr){
            // stream the contents of the Audio File into the Buffer
            // args: AudioSampleBuffer*, startSample, endSample, readerStartSample, useLeftChan, useRightChan
            reader->read (newBuffer->getAudioSampleBuffer(), 0, reader->lengthInSamples, 0, true, true);
            std::cout << "Samples in Buffer: " << newBuffer->getAudioSampleBuffer()->getNumSamples() << std::endl;
            
            fileBuffer = newBuffer;
        }
    } else {
        LOG("Sorry but the file you are trying to load does not exist :(");
    }
}

void GrrnlrrAudioProcessor::checkForBuffersToFree()
{
    
}

void GrrnlrrAudioProcessor::checkForRestoredPath()
{
    String path;
    path = restoredPath;
    if(path.isNotEmpty()){
        LOG("restoredPath: " << path);
		chosenPath.swapWith(path);
        restoredPath = "";
    }
}

void GrrnlrrAudioProcessor::checkForPathToOpen()
{
    String pathToOpen;
	pathToOpen.swapWith(chosenPath);

    if(pathToOpen.isNotEmpty()){
        LOG("pathToOpen: " << pathToOpen);
        filePath = pathToOpen;
        loadAudioFile(pathToOpen);
    }
}

void GrrnlrrAudioProcessor::run()
{
    while (! threadShouldExit()) {
        
        checkForRestoredPath();
        checkForPathToOpen();
        checkForBuffersToFree();
        
        // delete grains
        if( grainStack.size() > 0){
            for (int i=grainStack.size() - 1; i >= 0; --i) {
                // check if the grain has ended
                long long int grainEnd = grainStack[i].onset + grainStack[i].length;
                bool hasEnded = grainEnd < time;
                
                if(hasEnded) grainStack.remove(i); // [4]
            }
        }
        
        // handle MIDI
        Array<Array<int>> activeNotes;
        
        for(int i=0; i<128; i++){
            if(midiNotes[i] > 0){
                activeNotes.add( Array<int> {i, midiNotes[i] } );
            }
        }
        
        // add grains
        if(fileBuffer != nullptr){
            if(activeNotes.size()>0){
                // initialize nextGrainOnset to lie in the future
                if(nextGrainOnset == 0) nextGrainOnset = time;
                
                int numSamples = fileBuffer->getAudioSampleBuffer()->getNumSamples();
                
                // Transposition
                float midiNote = 60;
                midiNote = activeNotes[Random::getSystemRandom().nextInt(activeNotes.size())][0];
                midiNote = (midiNote - 61);
                
                float trans = *transParam + midiNote;
                trans += 1 + (*randTransParam * (Random::getSystemRandom().nextFloat() * 2 - 1));
                
                float ratio = pow (2.0, trans / 12.0);
                
                // Duration
                float dur = *durationParam * (1 + (*randDurParam * (Random::getSystemRandom().nextFloat() * 2 - 1)));
                // this mapping introduces some problems check later!
                //dur *= (1 / ratio);
                
                int schedDelay = 700;
                long long int onset = nextGrainOnset + schedDelay;
                
                // Length
                float density = *densityParam * (1 + (*randDensityParam * (Random::getSystemRandom().nextFloat() * 2 - 1)));
                int length = density * dur * sampleRate;
                
                // Position
                float randPosition = *randPosParam * (Random::getSystemRandom().nextFloat() - 0.5);
                int startPosition = (*positionParam + randPosition) * numSamples;
                startPosition = wrap(startPosition, 0, numSamples);

				// Spread
				Array<int> offsets;
				for (int i = 0; i < 2; ++i) offsets.add( Random::getSystemRandom().nextInt(Range<int>::between(0, *spreadParam * (numSamples)/2) ) );
				LOG("offsets: " << offsets[0] << " " << offsets[1]);
                
                // Envelope
                float envMid = *envMidParam;
                float envSus = *envSustainParam;
                float envCurve = *envCurveParam;
                
                // Amplitude
                float amp = *volumeParam;
                amp *= 1 + *randVolumeParam * (Random::getSystemRandom().nextFloat() * 2 - 1);
                
                nextGrainOnset = onset + (dur * sampleRate);
                
                grainStack.add( Grain(onset, length, startPosition, envMid, envSus, envCurve, ratio, amp, offsets) );
                
                double schedError = ((onset - schedDelay) - time) / sampleRate;
                dur += schedError;
                
                wait(dur * 1000);
            } else {
                // there are no held notes so we should reset the value for nextGrainOnset
                nextGrainOnset = 0;
                wait(100);
            }
        } else {
            wait(100);
        }
        
    }
}

//==============================================================================
void GrrnlrrAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int numSamplesInBlock = buffer.getNumSamples();
    
    // clear the buffer so we don't get any noise
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, numSamplesInBlock);
    
    processMidi(midiMessages, numSamplesInBlock);
    
    // make a copy of the Pointer to the fileBuffer so we are completely seperated
    // from the other thread
    ReferenceCountedBuffer::Ptr retainedBuffer (fileBuffer);
    
    // return from the function if there's no valid buffer
    if(retainedBuffer == nullptr) return;
    
    // finally get a AudioSampleBuffer we can use for processing
    AudioSampleBuffer* currentBuffer = retainedBuffer->getAudioSampleBuffer();
    
    const int numSamplesInFile  = currentBuffer->getNumSamples();
    
    const Array<Grain> localStack = grainStack;
    
    for (int s = 0; s < numSamplesInBlock; ++s) {
        for(int i=0; i < localStack.size(); ++i) {
            if (localStack[i].onset < time) {
                if (time < (localStack[i].onset + localStack[i].length)) {
                    localStack[i].process(buffer, *currentBuffer, buffer.getNumChannels(), numSamplesInBlock, numSamplesInFile, time);
                }
            }
        }
        // check for bad values
        for(int channel = 0; channel < buffer.getNumChannels(); ++channel ){
            float* channelData = buffer.getWritePointer(channel);
            float currentSample = channelData[s];
			for (int i = 0; i < localStack.size(); ++i) {
				if (currentSample > 1.0 || currentSample < -1.0) {
					LOG("WARN: Bad Sample Value: " << currentSample);
					LOG( "Dumping Grain Stack: "
						<< " Onset: " << localStack[i].onset
						<< " length: " << localStack[i].length
						<< " startPos: " << localStack[i].startPosition
						<< " envAttack: " << localStack[i].envAttack
						<< " envRelease: " << localStack[i].envRelease
						<< " envCurve: " << localStack[i].envCurve
						<< " Rate: " << localStack[i].rate
						<< " Amp: " << localStack[i].amp);
				}
			}
            channelData[s] = clip(currentSample, -1.0, 1.0);
        }
        
        ++time; // increment time
    }
}

//==============================================================================
void GrrnlrrAudioProcessor::processMidi(MidiBuffer& midiMessages, int numSamples)
{
    MidiBuffer::Iterator i (midiMessages);
    MidiMessage message;
    int time;
    
    while(i.getNextEvent(message, time))
    {
        if(message.isNoteOn()) {
            midiNotes[message.getNoteNumber()] = message.getVelocity();
            notify();
        }
        if(message.isNoteOff()) {
            midiNotes[message.getNoteNumber()] = 0;
        }
        if(message.isAllNotesOff()) {
            for(int i=0; i < 128; ++i)
                midiNotes[i] = 0;
        }
    }
}

//==============================================================================
AudioProcessorEditor* GrrnlrrAudioProcessor::createEditor()
{
    return new GrrnlrrAudioProcessorEditor (*this);
}

//==============================================================================
void GrrnlrrAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // Here's an example of how you can use XML to make it easy and more robust:
     LOG("\nSave Settings: ");
    
    // Create an outer XML element..
    XmlElement xml ("GRRNLRRPLUGINSETTINGS");
    
    // Store the values of all our parameters, using their param ID as the XML attribute
    for (int i = 0; i < getNumParameters(); ++i)
    {
        if (AudioProcessorParameterWithID* p = dynamic_cast<AudioProcessorParameterWithID*> (getParameters().getUnchecked(i)))
        {
            xml.setAttribute (p->paramID, p->getValue());
            
            LOG(p->paramID << " " << p->getValue());
            
        }
    }
    
    xml.setAttribute("FilePath", filePath);
    LOG("\nSave Path: " << filePath);
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
}

void GrrnlrrAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    LOG("Load Settings: ");
    
    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if (xmlState != nullptr)
    {
        // make sure that it's actually our type of XML object..
        if (xmlState->hasTagName ("GRRNLRRPLUGINSETTINGS"))
        {
            // Now reload our parameters..
            for (int i = 0; i < getNumParameters(); ++i)
            {
                if (AudioProcessorParameterWithID* p = dynamic_cast<AudioProcessorParameterWithID*> (getParameters().getUnchecked(i)))
                {
                    p->setValueNotifyingHost ((float) xmlState->getDoubleAttribute (p->paramID, p->getValue()));
                    
                    LOG(p->paramID << " " << p->getValue());
                }
            }
            restoredPath = xmlState->getStringAttribute("FilePath");
            LOG("\nLoad Path: " << restoredPath);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GrrnlrrAudioProcessor();
}
