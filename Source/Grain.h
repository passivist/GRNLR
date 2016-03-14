/**
   TODO:
      > working prototype
      > destroy Grain object when it ends
*/

#ifndef GRAIN_H_INCLUDED
#define GRAIN_H_INCLUDED

#include <chrono>
#include <iostream>
#include <vector>
#include <tuple>
#include <thread>
#include <math.h>


class grainEnvelope {
   grainEnvelope(){}
   ~grainEnvelope(){}

   int length;
   int currentPosition;
};

class Grain {
public:
  int lengthInSamples;
  int startPosition; 
  int currentPosition;
   /**
    CONSTRUCTOR
    */
   Grain()
   {
      startPosition = 0;
      lengthInSamples = 2048;
   };

   Grain(int start, int length){
      startPosition = start;
      lengthInSamples = length;
   };

   /**
    PROCESS FUNCTION:
    render an audio-block of a single grain for adding together later
    */
   void process(AudioSampleBuffer& buffer, AudioSampleBuffer& fileBuffer, int startSample, int numSamples)
   {
      int outputSamplesRemaining  = buffer.getNumSamples();
      /* not shure if this assignment is necissary but we may need it later */
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

class GrainStack {
  /** 
   the stack should consist of a vector of tuples of Grain objects and times (of type double)
   */
public:
  std::vector<std::tuple<Grain, double> > stack;
  void push(int startPosition, int length, double startTime)
  {
    stack.push_back(std::make_tuple(Grain(startPosition, length), startTime));
  }

  void pop()
  {
    stack.pop_back();
  }

  std::tuple<Grain, double> back()
  {
    return stack.back();
  }
};

#endif /* Grain_h */
