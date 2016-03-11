#include <vector>
#include <tuple>
#include <iostream>

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
};

class GrainStack {
  /** 
   the stack should consist of a vector of pairs of Grain objects and times (of type double)
   */
public:
  std::vector<std::tuple<Grain, double> > stack;
  void push(double startTime, int startPosition,  int length)
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

int main()
{
  GrainStack stack;
  stack.push(0.5, 124, 2056);

  auto slot = stack.back();
  std::cout << std::get<0>(slot).lengthInSamples << std::endl;
  return 0;
}
