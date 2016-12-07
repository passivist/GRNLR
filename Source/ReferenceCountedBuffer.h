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
    typedef ReferenceCountedObjectPtr<ReferenceCountedBuffer> Ptr; // [1]
    
    ReferenceCountedBuffer (const String& nameToUse,
                            int numChannels,
                            int numSamples) :   name (nameToUse),
                                                buffer (numChannels, numSamples) // [2]
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
    
    AudioSampleBuffer* getAudioSampleBuffer() // [3]
    {
        return &buffer;
    }
    
private:
    String name;
    AudioSampleBuffer buffer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReferenceCountedBuffer)
};

#endif  // REFERENCECOUNTEDBUFFER_H_INCLUDED
