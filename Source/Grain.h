#ifndef GRAIN_H_INCLUDED
#define GRAIN_H_INCLUDED

#include <math.h>

/** GRAIN CLASS
 In this file is the Grain class that describes an object holding
 all the information about a single grain.
 **/

class Grain {
    /* We first define all the member variables of the grain class. */
    // not shure if this couldn't all be private...
public:
    int grainLength;
    int startPosition;
    int onset;
    bool direction;
    float grainLengthRecip;
    float pitchRatio;
    
    float envCurve;
    
    float envAttack;
    float envSustain;
    float envRelease;
    
    float envAttackRecip;
    float envSustainRecip;
    float envReleaseRecip;
    
    float volume;
    
    bool hasEnded = false;
    
    /** CONSTRUCTOR
     The constructor is a member function of a class to create an instance
     (an object) of that class.
     
     We have 2 versions of the constructor. The first version has no arguments
     and is implemented mainly for safety and style reasons. It provides a
     default version of the grain object so the program doesn't crash when we
     try to create a grain without providing arguments.
     
     The second version will be the one actually used by the scheduling function
     in the PluginProcessor file.
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
        
        pitchRatio = 1;
        
        envAttackRecip  = 1/envAttack;
        envReleaseRecip = 1/(1 - envRelease);
        
        volume = 0.8;
    };
    
    Grain(int start, int length, int time, float trans, bool dir, float center, float sustain, float curve, float vol){
        startPosition = start;
        grainLength = length;
        onset = time;
        grainLengthRecip = 1.f / (float) grainLength;
        
        direction = dir;
        
        pitchRatio = pow (2.0, trans / 12.0);
        
        envCurve = curve;
        
        envAttack  = (1 - sustain) * center;
        envRelease = (1 - sustain) * (-1 * center) + 1;
        
        envAttackRecip  = 1/envAttack;
        envReleaseRecip = 1/(1 - envRelease);
        
        volume = vol;
    };
    
    /** PROCESS FUNCTION:
     render an audio-block of a single grain for and add them to the
     current block
     */
    
    void process(AudioSampleBuffer& currentBlock, AudioSampleBuffer& fileBuffer, int time, int numChannels, int blockSize)
    {
        float position = (time - onset) * pitchRatio;
        int filePosition;
        float floatPosition;
        
        /** DIRECTION
         We calculate the position to read from differently if we want to play foward or reverse.
         The important part here is the frame of reference:
            If the Grain should play forward the position to read at is defined as starting at the startPosition
            and continueing by adding the current position to that.
         
            If the grain should play backwards the position to read is defined as the end of the grain
            (startPosition + grainLength) and the current position is subtracted from that.
         */
        if(direction){
            filePosition = (startPosition + (int) position) % fileBuffer.getNumSamples();
            floatPosition = std::fmod(startPosition + position, fileBuffer.getNumSamples());
        } else {
            filePosition = abs(startPosition + grainLength - (int) position) % fileBuffer.getNumSamples();
            floatPosition = fabs(std::fmod(startPosition + grainLength - position, fileBuffer.getNumSamples()));

        }
        
        
        /** ENVELOPE
         */
        
        // this seems inefficient, most of the calculation is the same for every sample,
        // maybe calculate an array once at grain maybe calculate an array once at grain
        // creation and just index into it in here
        float envPos, gain;
        
        envPos = (time - onset) * grainLengthRecip;
        if(envPos <= envAttack){
            if(std::abs(envCurve) > 0.001){
                float aPos;
                
                aPos = envPos * envAttackRecip;
                
                double denom = 1.0f - exp(envCurve);
                double numer = 1.0f - exp(aPos * envCurve);
                
                gain = (numer/denom);
            } else {
                float aPos;
                
                aPos = envPos * envAttackRecip;
                gain = aPos;
            }
        } else if( envPos < envRelease){
            gain = 1.0;
        } else if( envPos >= envRelease ){
            if(std::abs(envCurve) > 0.001){
                float rPos;
                
                rPos = (envPos - envRelease) * envReleaseRecip;
                
                double denom = 1.0f - exp(-envCurve);
                double numer = 1.0f - exp(rPos * -envCurve);
                
                gain = (numer/denom) * (-1) + 1;
            } else {
                float rPos;
                
                rPos = (envPos - envRelease) * envReleaseRecip;
                gain = rPos * (-1) + 1;
            }
        }
        
        /** LINEAR INTERPOLATION
         */
        const float alpha = fabs(floatPosition - filePosition);
        const float invAlpha = 1.0f - alpha;
        /*
        std::cout   << "floatPos: " << floatPosition
                    << " filePos: " << filePosition
                    << " alpha: " << alpha
                    << " invAlpha: " << invAlpha
                    << std::endl;
        */
        /** AUDIO COPYING
         
         */
        for (int channel=0; channel < currentBlock.getNumChannels(); ++channel)
        {
            float* channelData = currentBlock.getWritePointer(channel);
            const float* fileData = fileBuffer.getReadPointer(channel % numChannels);
            
            /* We copy the data from the file into the right place in the buffer and add it to the previous data: */
            if(direction)
                channelData[ time % blockSize ] += (fileData[ (filePosition) ] * invAlpha + fileData[ (filePosition + 1)] * alpha) * gain * volume;
            else
                channelData[ time % blockSize ] += (fileData[ (filePosition) ] * invAlpha + fileData[ (filePosition - 1)] * alpha) * gain * volume;
            
        }
    }
};

#endif /* Grain_h */
