// async example
#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include <chrono>

// a non-optimized way of checking for prime numbers:
bool foo () {
  int time = 300;
  for (int i = 0; i<20; ++i)
    {
      std::cout << rand()%100 << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(time));
    }
  return true;
}

int main ()
{
  // call is_prime(313222313) asynchronously:
  std::future<bool> fut = std::async (is_prime,313222313);

  std::cout << "starting loop" << std::endl;
  
  bool ret = fut.get();      // waits for is_prime to return

  return 0;
}
