#include "WorkerThread.h"
#include "LinearTaskQueue.h"
#include <gtest/gtest.h>
#include <iostream>

using namespace tpool;
using namespace std;

TEST(WorkerThreadTestSuite, test_Ctor)
{
  {
    TaskQueueBase::Ptr q(new LinearTaskQueue);
    WorkerThread t(q);

  }
  cout << "worker thread end" << endl;

}
