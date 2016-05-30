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
    
    float envelopeNext (int position)
    {
        float gain;
        float angle;
        angle = (float)(position + currentPosition) * grainLengthRecip;
        gain = sin(angle * float_Pi);
        if(gain < 0.00001) gain = 0;
        
        return gain;
    }
    
    /**
     PROCESS FUNCTION:
     render an audio-block of a single grain for and add them to the
     current block
     */
    void process(AudioSampleBuffer& currentBlock, AudioSampleBuffer& fileBuffer, int offset)
    {
        int blockSize  = currentBlock.getNumSamples();
        int filePosition = currentPosition + startPosition;
        int fileNumChannels = fileBuffer.getNumChannels();
        
        if(currentPosition < grainLength) {
            int grainSamplesRemaining = grainLength - currentPosition;
            int samplesThisBlock = jmin(blockSize, grainSamplesRemaining);
            
            if(samplesThisBlock < 0) samplesThisBlock = 0;
            
            for (int channel=0; channel < currentBlock.getNumChannels(); ++channel)
            {
                float gain = 0;
                
                // the guts:
                for(int i=0; i <= samplesThisBlock; ++i)
                {
                    float* channelData = currentBlock.getWritePointer(channel % fileNumChannels);
                    const float* fileData = fileBuffer.getReadPointer(channel % fileNumChannels);
                    
                    gain = envelopeNext(i);
                    
                    // We copy the data from the file into the right place in the buffer and add it to the previous data:
                    channelData[i+offset] += fileData[ (i+filePosition) % fileBuffer.getNumSamples() ] * gain;
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
