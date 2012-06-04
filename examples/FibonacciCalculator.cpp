#include "FixedThreadPool.h"
#include <iostream>

using namespace std;
using namespace tpool;

void InputPrompt(const unsigned int cnt)
{
  cout << "[" << cnt << "] In: ";
}

unsigned int GetFibonacciNumber(const unsigned int i)
{
  if (i < 2)
    {
      return i;
    }
  else
    {
      return GetFibonacciNumber(i - 1) + GetFibonacciNumber(i - 2);
    }
}

void OutputFibonacciNumber(const unsigned int cnt, const unsigned int i)
{
  cout << "[" << cnt << "] Out: " << GetFibonacciNumber(i) << endl;
}

void SyncLoop()
{
  unsigned int i = 0, cnt = 1;

  InputPrompt(cnt);
  while(cin >> i)
    {
      OutputFibonacciNumber(cnt, i);
      InputPrompt(++cnt);
    }
}

void AsyncLoop()
{
  unsigned int i = 0, cnt = 1;
  LFixedThreadPool threadPool;

  InputPrompt(cnt);
  while(cin >> i)
    {
      threadPool.AddTask(boost::bind(&OutputFibonacciNumber, cnt, i));
      InputPrompt(++cnt);
    }
}

int main(int argc, char *argv[])
{
  // SyncLoop();
  AsyncLoop();
  
  return 0;
}
