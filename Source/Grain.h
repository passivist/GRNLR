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
    
    float envCenter;
    float envSustain;
    float envCurve;
    
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
        envCenter = 0.5;
        envSustain = 0;
        envCurve = 1;
    };
    
    Grain(int start, int length, int time, float center, float sustain, float curve){
        startPosition = start;
        grainLength = length;
        startTime = time;
        currentPosition = 0;
        grainLengthRecip = 1.f / (float) grainLength;
        envCenter = center;
        envSustain = sustain;
        envCurve = curve;
    };
    
    /**
     PROCESS FUNCTION:
     render an audio-block of a single grain for and add them to the
     current block
     */
    void process(AudioSampleBuffer& currentBlock, AudioSampleBuffer& fileBuffer, int offset)
    {
        int blockSize  = currentBlock.getNumSamples();
        
        int fileNumChannels = fileBuffer.getNumChannels();
        int filePosition = currentPosition + startPosition;
        
        if(currentPosition < grainLength) {
            int grainSamplesRemaining = grainLength - currentPosition;
            int samplesThisBlock = jmin(blockSize, grainSamplesRemaining);
            samplesThisBlock -= offset;
            
            if(samplesThisBlock < 0) samplesThisBlock = 0;
            
            std::cout << samplesThisBlock << std::endl;
            
            float gain [samplesThisBlock];
            
            // ENVELOPE
            for (int i=0; i<samplesThisBlock; ++i)
            {
                gain[i] = (i + currentPosition) * grainLengthRecip;
            }
            
            for (int channel=0; channel < currentBlock.getNumChannels(); ++channel)
            {
                float* channelData = currentBlock.getWritePointer(channel);
                const float* fileData = fileBuffer.getReadPointer(channel % fileNumChannels);
                
                // the guts:
                for(int i=0; i < samplesThisBlock; i++)
                {
                    // We copy the data from the file into the right place in the buffer and add it to the previous data:
                    channelData[i+offset] += fileData[ (i+filePosition) % fileBuffer.getNumSamples() ] * gain[i];
                    //channelData[i] += gain[i];
                }
            }
            
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
