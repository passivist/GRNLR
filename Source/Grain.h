#ifndef GRAIN_H_INCLUDED
#define GRAIN_H_INCLUDED

#include <math.h>

class Grain {
public:
    int grainLength;
    int startPosition;
    int currentPosition;
    int startTime;
    float grainLengthRecip;
    bool hasEnded = false;
    /**
     CONSTRUCTOR
     */
    Grain()
    {
        startPosition = 0;
        grainLength = 2048;
        currentPosition = 0;
        grainLengthRecip = 1.f / (float) grainLength;
        startTime = 0;
    };
    
    Grain(int start, int length, int time){
        startPosition = start;
        grainLength = length;
        startTime = time;
        currentPosition = 0;
        grainLengthRecip = 1.f / (float) grainLength;
    };
    
    /**
     PROCESS FUNCTION:
     render an audio-block of a single grain for and add them to the
     current block
     */
    void process(AudioSampleBuffer& currentBlock, AudioSampleBuffer& fileBuffer, int offset)
    {
        int blockSize  = currentBlock.getNumSamples();
        int filePosition = currentPosition + startPosition;
        
        if(currentPosition < grainLength) {
            int grainSamplesRemaining = grainLength - currentPosition;
            int samplesThisBlock = jmin(blockSize, grainSamplesRemaining);
            
            if(samplesThisBlock < 0) samplesThisBlock = 0;

            for (int channel=0; channel < currentBlock.getNumChannels(); ++channel)
            {
                float gain = 0;
                float angle = 0;
                
                float* channelData = currentBlock.getWritePointer(channel);
                const float* fileData = fileBuffer.getReadPointer(channel);

                // the guts:
                for(int i=0; i <= samplesThisBlock; ++i)
                {       
                    angle = (float)(i + currentPosition) * grainLengthRecip;
                    gain = sin(angle * float_Pi);
                    if(gain < 0.00001) gain = 0;
                    
                    // We copy the data from the file into the right place in the buffer and add it to the previous data:
                    channelData[i+offset] += fileData[ (i+filePosition) % fileBuffer.getNumSamples() ] * gain;
                    //std::cout << channelData[i+offset] << ", ";
                }
            }

            /*
            std::cout << "block: " << samplesThisBlock
                      << " grainPosition " << currentPosition
                      << " filePosition: " << filePosition
                      << " length: " << grainLength
                      << " Offset: "  <<  offset
                      << std::endl;
            */
            
            //std::cout << std::endl;
            
            // update grain position
            currentPosition += samplesThisBlock;
        } else {
            // set the hasEnded property of the grain to true so it
            // will be deleted on the next block.
            hasEnded = true;
        }
    }
};

#endif /* Grain_h */
