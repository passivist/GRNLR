#ifndef GRAIN_H_INCLUDED
#define GRAIN_H_INCLUDED

#include <math.h>

class Grain {
public:
    int lengthInSamples;
    int startPosition;
    int currentPosition;
    bool hasEnded = false;
    /**
     CONSTRUCTOR
     */
    Grain()
    {
        startPosition = 0;
        lengthInSamples = 2048;
        currentPosition = 0;
    };
    
    Grain(int start, int length){
        startPosition = start;
        lengthInSamples = length;
        currentPosition = 0;
    };
    
    /**
     PROCESS FUNCTION:
     render an audio-block of a single grain for adding together later
     */
    // work on precision: grain values are only updated every audioblock, so some errors occur
    // we should implement offsets
    void process(AudioSampleBuffer& buffer, AudioSampleBuffer& fileBuffer)
    {
        int outputSamplesRemaining  = buffer.getNumSamples();
        /* not shure if this assignment is necessary but we may need it later */
        int outputSamplesOffset     = 0;
        int position = currentPosition + startPosition;
        float lengthInSamplesRecip = 1.f / (float)lengthInSamples;
        //std::cout << lengthInSamplesRecip << "recip!" << std::endl;
        
        if(currentPosition < lengthInSamples) {
            int bufferSamplesRemaining = fileBuffer.getNumSamples() - (currentPosition + startPosition);
            int samplesThisBlock = jmin(outputSamplesRemaining, bufferSamplesRemaining);
            
            for (int channel=0; channel < buffer.getNumChannels(); ++channel) {
                
                buffer.addFrom( channel,
                               outputSamplesOffset,
                               fileBuffer,
                               channel % fileBuffer.getNumChannels(),
                               position % fileBuffer.getNumSamples(),
                               samplesThisBlock );
                
                float gain = 0;
                float angle = 0;
                
                float* const channelData = buffer.getWritePointer(channel);
                
                /*
                 std::cout << "block: " << samplesThisBlock
                 << " position: " << position
                 << " length: " << lengthInSamples
                 << std::endl;
                 */
                
                for(int i=0; i < outputSamplesRemaining; ++i)
                {
                    angle = (float)(i+currentPosition)*lengthInSamplesRecip;
                    gain = sin(angle * float_Pi);
                    //if(channel==0) std::cout << gain << ", ";
                    channelData[i] *= gain;
                }
            }
            //std::cout << "\n \n"  << std::endl;
            
            currentPosition         += outputSamplesRemaining;
            outputSamplesRemaining  -= samplesThisBlock;
            outputSamplesOffset     += samplesThisBlock;
            
            
            
        } else {
            /**
             here we should delete the grain
             or at least have some kind of callback or change in some property to signal
             that the grain has ended
             */
            
            //std::cout << "+++ grain has ended +++" << std::endl;
            hasEnded = true;
        }
    }
};

#endif /* Grain_h */
