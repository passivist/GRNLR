#ifndef GRAIN_H_INCLUDED
#define GRAIN_H_INCLUDED
/*
   TODO:
      > working prototype
      > destroy Grain object when it ends
*/

class grainEnvelope {
   grainEnvelope(){}
   ~grainEnvelope(){}

   int length;
   int currentPosition;
};

class Grain {
public:
   Grain() {}
   ~Grain() {}
   
   int lengthInSamples;
   int startPosition;
   int currentPosition;

   // load the appropriate number of samples into the current audio-block
   void process(AudioSampleBuffer& buffer, AudioSampleBuffer& fileBuffer, int startSample, int numSamples)
   {
      int outputSamplesRemaining  = buffer.getNumSamples();
      int outputSamplesOffset     = 0;
      
      if(currentPosition < numSamples) {
         int bufferSamplesRemaining = fileBuffer.getNumSamples() - currentPosition;
         int samplesThisBlock = jmin(outputSamplesRemaining, bufferSamplesRemaining);
         
         for (int channel=0; channel < buffer.getNumChannels(); ++channel) {
            buffer.copyFrom( channel,
                            outputSamplesOffset,
                            fileBuffer,
                            channel % fileBuffer.getNumChannels(),
                            (currentPosition + startSample) % fileBuffer.getNumSamples(),
                            samplesThisBlock );
         }
         
         outputSamplesRemaining  -= samplesThisBlock;
         outputSamplesOffset     += samplesThisBlock;
         currentPosition         += samplesThisBlock;
      } else {
         currentPosition = 0;
      }
   }
};


#endif /* Grain_h */
