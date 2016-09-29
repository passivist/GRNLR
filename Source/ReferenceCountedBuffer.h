/*
 ==============================================================================
 
 This class was taken from the Tutorial "Looping audio using the AudioSampleBuffer class (advanced)"
 at https://www.juce.com/doc/tutorial_looping_audio_sample_buffer_advanced
 
 ==============================================================================
 */

#ifndef REFERENCECOUNTEDBUFFER_H_INCLUDED
#define REFERENCECOUNTEDBUFFER_H_INCLUDED

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



#endif  // REFERENCECOUNTEDBUFFER_H_INCLUDED
