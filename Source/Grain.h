#ifndef GRAIN_H_INCLUDED
#define GRAIN_H_INCLUDED

#include <math.h>

class Grain {
public:
    int grainLength;
    int startPosition;
    int onset;
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
        grainLengthRecip = 1.f / (float) grainLength;
        onset = 0;
        envCenter = 0.5;
        envSustain = 0;
        envCurve = 1;
    };
    
    Grain(int start, int length, int time, float center, float sustain, float curve){
        startPosition = start;
        grainLength = length;
        onset = time;
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
    void process(AudioSampleBuffer& currentBlock, AudioSampleBuffer& fileBuffer, int time, int numChannels, int blockSize)
    {
        int filePosition = startPosition + (time % grainLength);
        
        float gain;
        
        // ENVELOPE
        gain = (time % grainLength) * grainLengthRecip;
        
        
        for (int channel=0; channel < currentBlock.getNumChannels(); ++channel)
        {
            float* channelData = currentBlock.getWritePointer(channel);
            const float* fileData = fileBuffer.getReadPointer(channel % numChannels);
            
            // We copy the data from the file into the right place in the buffer and add it to the previous data:
            channelData[ time % blockSize ] += fileData[ (filePosition) % fileBuffer.getNumSamples() ] * gain;
            //channelData[i] += gain[i];
        }
    }
};

#endif /* Grain_h */
