#include "Scheduler.h"

void schedule(int time)
{
  stack.push(rand() % 2000, rand() % 6000, time );
  std::this_thread::sleep_for(std::chrono::milliseconds(time));
}

int main()
{
  srand(time(NULL));
  for (int i=0; i<20; ++i)
    { 
      schedule( (rand() % 4000) + 100);
      std::tuple<Grain, double> slot = stack.back();
      std::cout << std::get<0>(slot).startPosition << " "
		<< std::get<0>(slot).lengthInSamples << " "
		<< std::get<1>(slot) << std::endl; 
    }
  return 0;
}
