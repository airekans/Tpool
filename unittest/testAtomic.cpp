#include "Atomic.h"
#include <gtest/gtest.h>

using namespace tpool;

TEST(Atomic, test_Ctor)
{
  Atomic<int> i(1);

  ASSERT_EQ(1, (int)i);
}

TEST(Atomic, test_copy_ctor)
{
  Atomic<int> i1(1);
  Atomic<int> i2(i1);

  ASSERT_EQ(1, (int)i1);
  ASSERT_EQ(1, (int)i2);
}

TEST(Atomic, test_assignment_T)
{
  Atomic<int> i(0);
  i = 1;

  ASSERT_EQ(1, (int)i);
}

TEST(Atomic, test_assignment_Atomic)
{
  Atomic<int> i1(0);
  Atomic<int> i2(1);
  i1 = i2;

  ASSERT_EQ(1, i1);
  ASSERT_EQ(1, i2);
}
