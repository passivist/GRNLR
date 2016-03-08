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
   int lengthInSamples;
   int startPosition;
public:
   int currentPosition;
   Grain()
   {
      startPosition = 0;
      lengthInSamples = 2048;
   };

   Grain(int start, int length){
      startPosition = start;
      lengthInSamples = length;
   };

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

// hier mal mit herumspielen in seperatem file
class grainStack {
public:
   std::vector <Grain> grains;
   std::vector <double> times;

   void push(double startTime, int startPosition,  int length)
   {
      Grain grain(startPosition, length);
      grains.push_back(grain);
      times.push_back(startTime);
   }

   void pop()
   {
      grains.pop_back();
      times.pop_back();
   } 
};

#endif /* Grain_h */
