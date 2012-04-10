#include "Thread.h"
#include <iostream>

using namespace tpool;
using namespace std;

class TestThread : public Thread<TestThread> {
public:
  virtual void Entry()
  {
    for (int i = 0; i < 20; ++i)
      {
	cout << "This is TestThread: " << i << endl;
      }
  }


};

int main(int argc, char** argv)
{
  TestThread t;

  t.Run();

  for (int i = 0; i < 20; ++i)
    {
      cout << "This is main: " << i << endl;
    }

  sleep(1);
  
  return 0;
}
