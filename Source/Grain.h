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
    void process(AudioSampleBuffer& buffer, AudioSampleBuffer& fileBuffer)
    {
        int outputSamplesRemaining  = buffer.getNumSamples();
        /* not shure if this assignment is necessary but we may need it later */
        int outputSamplesOffset     = 0;
        int position = currentPosition + startPosition;
        
        //std::cout << "processing!" << std::endl;
        if(currentPosition < lengthInSamples) {
            int bufferSamplesRemaining = lengthInSamples - (currentPosition + startPosition);
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
		// der übergang zwischen blocks scheint zu funktionieren, aber wenn das grain endet auf zu hoher amplitude
		// ~0.8
		// angle endet  0.581692; ???
		// hat es etwas mit der logik zutun wie das programm entscheidet ob das grain den block spielt oder nicht
		// kann eigentlich nicht sein: jmin entscheidet ob der ganze block oder der rest des samples gespielt
		// wird.
		// trotzdem ist samplesThis block immer 2048?? wieso?
		// samplesThis block ist in mindestes in einem Fall zu lang!!
		// scheint gelöst, oben wurde die länge des Blocks nicht auf die länge des Grains sondern auf die länge des
		// samples bezogen.

		/*
		std::cout << "block: " << samplesThisBlock
			  << " position: " << position
			  << " length: " << lengthInSamples
			  << std::endl;
		*/
                for(int i=0; i<samplesThisBlock; ++i)
                {
                    angle = (float)((i+position)%lengthInSamples)/lengthInSamples;
                    gain = sin(angle * float_Pi);
                    //std::cout << angle << "; ";
                    channelData[i] *= gain;
                }
            }
	    // std::cout << "\n \n \n"  << std::endl;
            outputSamplesRemaining  -= samplesThisBlock;
            outputSamplesOffset     += samplesThisBlock;
            currentPosition         += samplesThisBlock;

	    
        } else {
	  /** 
	      here we should delete the grain
	      or at least have some kind of callback or change in some property to signal
	      that the grain has ended
	  */

	  //std::cout << "+++ grain has ended +++" << std::endl;
	  hasEnded = true;
	  currentPosition = 0;
        }
    }
};

#endif /* Grain_h */
