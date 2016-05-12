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
        grainLengthRecip = 1.f / (float)grainLength;
        startTime = 0;
    };
    
    Grain(int start, int length, int time){
        startPosition = start;
        grainLength = length;
        startTime = time;
        currentPosition = 0;
        grainLengthRecip = 1.f / (float)grainLength;
    };
    
    /**
     PROCESS FUNCTION:
     render an audio-block of a single grain for and add them to the
     current block
     */
    // work on precision: grain values are only updated every audioblock, so some errors occur
    // we should implement offsets via outputSamplesOffset
    void process(AudioSampleBuffer& currentBlock, AudioSampleBuffer& fileBuffer, int offset)
    {
        int blockSize  = currentBlock.getNumSamples();
        int filePosition = currentPosition + startPosition; // filePosition
        //std::cout << "reciprocal of length:" << grainLengthRecip << std::endl;

        //std::cout << "Offset: "  << offset << std::endl;
        offset = 0;
        
        if(currentPosition < grainLength) {
            int bufferSamplesRemaining = fileBuffer.getNumSamples() - (currentPosition + startPosition);
            int samplesThisBlock = jmin(blockSize, bufferSamplesRemaining);
            samplesThisBlock -= offset;

            // maybe this should be implemented like the envelope
            // e.g. not with .addFrom but with read and write pointers...
            for (int channel=0; channel < currentBlock.getNumChannels(); ++channel)
            {
                currentBlock.addFrom( channel,
                                      offset,
                                      fileBuffer,
                                      channel % fileBuffer.getNumChannels(),
                                      filePosition % fileBuffer.getNumSamples(),
                                      samplesThisBlock );
                
                float gain = 0;
                float angle = 0;
                
                float* const channelData = currentBlock.getWritePointer(channel);
                
                /*
                 std::cout << "block: " << samplesThisBlock
                 << " filePosition: " << filePosition
                 << " length: " << grainLength
                 << std::endl;
                 */
                
                for(int i=0; i < blockSize; ++i)
                {
                    if(i > offset){   
                        angle = (float)((i - offset) + currentPosition) * grainLengthRecip;
                        gain = sin(angle * float_Pi);
                        channelData[i] *= gain;
                    }
                }
            }
            //std::cout << "\n \n"  << std::endl;
            
            currentPosition         += samplesThisBlock;
            blockSize               -= samplesThisBlock;
        } else {
            // set the hasEnded property of the grain to true so it
            // will be deleted on the next block.
            hasEnded = true;
        }
    }
};

#endif /* Grain_h */
