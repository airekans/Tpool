#include "FixedThreadPool.h"
#include <gtest/gtest.h>

using namespace tpool;

TEST(FixedThreadPoolTestSuite, test_Construction)
{
  {
    LFixedThreadPool threadPool;
    EXPECT_EQ(10, threadPool.GetThreadNum());
  }
  
  {
    LFixedThreadPool threadPool(5);
    EXPECT_EQ(5, threadPool.GetThreadNum());
  }
}
