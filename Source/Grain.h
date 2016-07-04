#ifndef GRAIN_H_INCLUDED
#define GRAIN_H_INCLUDED

#include <math.h>

class Grain {
public:
    int grainLength;
    int startPosition;
    int onset;
    float grainLengthRecip;
    
    float envCurve;
    
    float envAttack;
    float envSustain;
    float envRelease;
    
    float envAttackRecip;
    float envSustainRecip;
    float envReleaseRecip;
    
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
        
        envCurve = 1;
        envAttack = 0.4;
        envRelease = 0.4;
        
        envAttackRecip  = 1/envAttack;
        envReleaseRecip = 1/(1 - envRelease);
    };
    
    Grain(int start, int length, int time, float center, float sustain, float curve){
        startPosition = start;
        grainLength = length;
        onset = time;
        grainLengthRecip = 1.f / (float) grainLength;
        envCurve = curve;
        
        envAttack  = (1 - sustain) * center;
        envRelease = (1 - sustain) * (-1 * center) + 1;
        
        envAttackRecip  = 1/envAttack;
        envReleaseRecip = 1/(1 - envRelease);
    };
    
    /**
     PROCESS FUNCTION:
     render an audio-block of a single grain for and add them to the
     current block
     */
    void process(AudioSampleBuffer& currentBlock, AudioSampleBuffer& fileBuffer, int time, int numChannels, int blockSize)
    {
        unsigned int filePosition = startPosition + ((time - onset) % grainLength);
        
        // ENVELOPE
        // this is inefficient, most of the calculation is the same for every sample, maybe calculate an array once at grain
        // creation and just index into it in here
        float envPos, gain;
        envPos = (time - onset) * grainLengthRecip;
        if(envPos <= envAttack){
            float aPos;
            aPos = envPos * envAttackRecip;
            gain = aPos;
        } else if( envAttack < envPos < envRelease){
            gain = 1;
        } else if( envPos >= envRelease ){
            float rPos;
            rPos = (envPos - envRelease) * envReleaseRecip;
            gain = rPos * (-1) + 1;
        }

        // AUDIO COPYING
        for (int channel=0; channel < currentBlock.getNumChannels(); ++channel)
        {
            float* channelData = currentBlock.getWritePointer(channel);
            const float* fileData = fileBuffer.getReadPointer(channel % numChannels);
            
            // We copy the data from the file into the right place in the buffer and add it to the previous data:
            channelData[ time % blockSize ] += fileData[ (filePosition) % fileBuffer.getNumSamples() ] * gain;
        }
        
        
    }
};

#endif /* Grain_h */
