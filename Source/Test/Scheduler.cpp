#include <thread>
#include <chrono>
#include <iostream>

void scheduleGrain(int startTime, int length, int time)
{
  std::cout << "start: " << startTime
	    << " length: " << length
	    << " time: " << time << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(time));
}

void schedule()
{
  for (int i = 0; i < 10; ++i)
    {      
      int startTime = rand() % 6000;
      int length = rand() % 4000;
      int time = rand() % 1000;
      std::thread thread(scheduleGrain, startTime, length, time);
      thread.join();
    }
}


int main()
{
  srand(time(NULL));

  std::thread thread(schedule);

  for (int i=0; i<20; ++i)
    {
      int time = rand() % 500;
      std::cout << "hello" << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(time));
    };

  thread.join();

  return 0;
}
