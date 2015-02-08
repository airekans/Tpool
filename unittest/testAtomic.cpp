#include "Atomic.h"
#include "Thread.h"
#include <gtest/gtest.h>
#include <string>

using namespace std;
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

TEST(Atomic, test_self_assignment)
{
  Atomic<string> s("abc");
  s = s;

  ASSERT_EQ("abc", (string)s);
}

namespace {
  struct IncCounter {
    int counter;

    IncCounter() : counter(0) {}

    IncCounter& operator=(const IncCounter& i)
    {
      if (this != &i)
	{
	  ++counter;
	}
      return *this;
    }
  };
  
  struct ThreadFunc {
    Atomic<IncCounter>& m_data;

    ThreadFunc(Atomic<IncCounter>& atom)
      : m_data(atom)
    {}

    void operator()()
    {
      m_data = IncCounter();
    }
  };
}

TEST(Atomic, test_concurrent_set)
{
  Atomic<IncCounter> i((IncCounter()));
  {
    Thread t((ThreadFunc(i)));
    i = IncCounter();
  }

  ASSERT_EQ(2, ((IncCounter)i).counter);
}

namespace {

template<typename T, unsigned times>
struct AssignFunc {
    Atomic<T>& m_lhs;
    Atomic<T>& m_rhs;

    AssignFunc(Atomic<T>& lhs, Atomic<T>& rhs)
    : m_lhs(lhs), m_rhs(rhs)
    {}

    void operator()()
    {
        for (unsigned i = 0; i < times; ++i)
        {
            m_lhs = m_rhs;
        }
    }
};


}

TEST(Atomic, test_int_deadlock)
{
    Atomic<int> i1(123);
    Atomic<int> i2(321);

    {
        Thread t1((AssignFunc<int, 1000>(i1, i2)));
        Thread t2((AssignFunc<int, 1000>(i2, i1)));
    }

    ASSERT_TRUE(i1 == 123 || i1 == 321);
    ASSERT_TRUE(i2 == 123 || i2 == 321);
}

TEST(Atomic, test_string_deadlock)
{
    Atomic<string> s1("123");
    Atomic<string> s2("321");
    s1 = "123";

    {
        Thread t1((AssignFunc<string, 1000>(s1, s2)));
        Thread t2((AssignFunc<string, 1000>(s2, s1)));
    }

//    const string s1_s = static_cast<string>(s1);
//    const string s2_s = static_cast<string>(s2);
    ASSERT_TRUE(s1 == "123" || s1 == "321");
    ASSERT_TRUE(s2 == "123" || s2 == "321");
}


namespace {

template<typename T, unsigned times>
struct PreIncThreadFunc
{
    Atomic<T>& m_num;

    explicit PreIncThreadFunc(Atomic<T>& num)
    : m_num(num)
    {}

    void operator()()
    {
        for (unsigned i = 0; i < times; ++i)
        {
            ++m_num;
        }
    }
};

template<typename T, unsigned times>
struct PostIncThreadFunc
{
    Atomic<T>& m_num;

    explicit PostIncThreadFunc(Atomic<T>& num)
    : m_num(num)
    {}

    void operator()()
    {
        for (unsigned i = 0; i < times; ++i)
        {
            m_num++;
        }
    }
};

template<typename T, unsigned times>
struct PreDecThreadFunc
{
    Atomic<T>& m_num;

    explicit PreDecThreadFunc(Atomic<T>& num)
    : m_num(num)
    {}

    void operator()()
    {
        for (unsigned i = 0; i < times; ++i)
        {
            --m_num;
        }
    }
};

template<typename T, unsigned times>
struct PostDecThreadFunc
{
    Atomic<T>& m_num;

    explicit PostDecThreadFunc(Atomic<T>& num)
    : m_num(num)
    {}

    void operator()()
    {
        for (unsigned i = 0; i < times; ++i)
        {
            m_num--;
        }
    }
};

template<typename T>
class BasicAtomicNumTypeTest : public ::testing::Test {

};

}

typedef ::testing::Types<
        char, unsigned char,
        short, unsigned short,
        int, unsigned int,
        long, unsigned long,
        long long, unsigned long long> BasicAtomicTypes;
TYPED_TEST_CASE(BasicAtomicNumTypeTest, BasicAtomicTypes);

TYPED_TEST(BasicAtomicNumTypeTest, test_concurrent_preinc)
{
    typedef PreIncThreadFunc<TypeParam, 20> PreIncFunc;

    Atomic<TypeParam> num(0);
    {
        Thread t1((PreIncFunc(num)));
        Thread t2((PreIncFunc(num)));
        Thread t3((PreIncFunc(num)));
    }

    ASSERT_EQ(60, static_cast<TypeParam>(num));
}

TYPED_TEST(BasicAtomicNumTypeTest, test_concurrent_postinc)
{
    typedef PostIncThreadFunc<TypeParam, 20> PostIncFunc;

    Atomic<TypeParam> num(0);
    {
        Thread t1((PostIncFunc(num)));
        Thread t2((PostIncFunc(num)));
        Thread t3((PostIncFunc(num)));
    }

    ASSERT_EQ(60, static_cast<TypeParam>(num));
}

TYPED_TEST(BasicAtomicNumTypeTest, test_concurrent_predec)
{
    typedef PreDecThreadFunc<TypeParam, 20> PreDecFunc;

    Atomic<TypeParam> num(100);
    {
        Thread t1((PreDecFunc(num)));
        Thread t2((PreDecFunc(num)));
        Thread t3((PreDecFunc(num)));
    }

    ASSERT_EQ(40, static_cast<TypeParam>(num));
}

TYPED_TEST(BasicAtomicNumTypeTest, test_concurrent_postdec)
{
    typedef PostDecThreadFunc<TypeParam, 20> PostDecFunc;

    Atomic<TypeParam> num(100);
    {
        Thread t1((PostDecFunc(num)));
        Thread t2((PostDecFunc(num)));
        Thread t3((PostDecFunc(num)));
    }

    ASSERT_EQ(40, static_cast<TypeParam>(num));
}

namespace {

template<typename T>
class AdvancedAtomicNumTypeTest : public ::testing::Test {

};

}

typedef ::testing::Types<
        short, unsigned short,
        int, unsigned int,
        long, unsigned long,
        long long, unsigned long long> AdvAtomicTypes;
TYPED_TEST_CASE(AdvancedAtomicNumTypeTest, AdvAtomicTypes);

TYPED_TEST(AdvancedAtomicNumTypeTest, test_concurrent_preinc)
{
    typedef PreIncThreadFunc<TypeParam, 2000> PreIncFunc;

    Atomic<TypeParam> num(0);
    {
        Thread t1((PreIncFunc(num)));
        Thread t2((PreIncFunc(num)));
        Thread t3((PreIncFunc(num)));
    }

    ASSERT_EQ(6000, static_cast<TypeParam>(num));
}

TYPED_TEST(AdvancedAtomicNumTypeTest, test_concurrent_postinc)
{
    typedef PostIncThreadFunc<TypeParam, 2000> PostIncFunc;

    Atomic<TypeParam> num(0);
    {
        Thread t1((PostIncFunc(num)));
        Thread t2((PostIncFunc(num)));
        Thread t3((PostIncFunc(num)));
    }

    ASSERT_EQ(6000, static_cast<TypeParam>(num));
}

TYPED_TEST(AdvancedAtomicNumTypeTest, test_concurrent_predec)
{
    typedef PreDecThreadFunc<TypeParam, 2000> PreDecFunc;

    Atomic<TypeParam> num(10000);
    {
        Thread t1((PreDecFunc(num)));
        Thread t2((PreDecFunc(num)));
        Thread t3((PreDecFunc(num)));
    }

    ASSERT_EQ(4000, static_cast<TypeParam>(num));
}

TYPED_TEST(AdvancedAtomicNumTypeTest, test_concurrent_postdec)
{
    typedef PostDecThreadFunc<TypeParam, 2000> PostDecFunc;

    Atomic<TypeParam> num(10000);
    {
        Thread t1((PostDecFunc(num)));
        Thread t2((PostDecFunc(num)));
        Thread t3((PostDecFunc(num)));
    }

    ASSERT_EQ(4000, static_cast<TypeParam>(num));
}


