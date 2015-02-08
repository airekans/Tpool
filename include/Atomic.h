// -*- mode: c++ -*-
#ifndef _TPOOL_ATOMIC_H_
#define _TPOOL_ATOMIC_H_

#include "Mutex.h"
#ifdef _DEBUG_
#include <iostream>
#endif

namespace tpool {

namespace detail {

template<typename T>
struct ParamTraits {
    typedef const T& ParamType;
    typedef const T& RetType;
};

#define DEFINE_NATIVE_PARAM_TRAITS(type) \
        template<> \
        struct ParamTraits<type> { \
            typedef type ParamType; \
            typedef type RetType; \
        }

DEFINE_NATIVE_PARAM_TRAITS(bool);
DEFINE_NATIVE_PARAM_TRAITS(char);
DEFINE_NATIVE_PARAM_TRAITS(unsigned char);
DEFINE_NATIVE_PARAM_TRAITS(short);
DEFINE_NATIVE_PARAM_TRAITS(unsigned short);
DEFINE_NATIVE_PARAM_TRAITS(int);
DEFINE_NATIVE_PARAM_TRAITS(unsigned int);
DEFINE_NATIVE_PARAM_TRAITS(long);
DEFINE_NATIVE_PARAM_TRAITS(unsigned long);
DEFINE_NATIVE_PARAM_TRAITS(long long);
DEFINE_NATIVE_PARAM_TRAITS(unsigned long long);
DEFINE_NATIVE_PARAM_TRAITS(float);
DEFINE_NATIVE_PARAM_TRAITS(double);

}  // namespace detail

  template<typename T>
  class Atomic {
    typedef typename detail::ParamTraits<T>::ParamType ParamType;
    typedef typename detail::ParamTraits<T>::RetType RetType;

  public:
    explicit Atomic(ParamType i);
    Atomic(const Atomic& atom);

    Atomic& operator=(ParamType i);
    Atomic& operator=(const Atomic& atom);
    bool CompareAndSet(ParamType compare, ParamType newValue);
    operator T() const;
    RetType GetData() const;
    RetType GetRawData() const;
    bool Equal(const Atomic& other) const;
    bool Equal(ParamType other) const;

  private:
    mutable sync::Mutex m_mutex;
    T m_data;
  };

  // Partial specialization
  template<typename T>
  class _AtomicNativeNumber
  {
  public:
    explicit _AtomicNativeNumber(const T data)
    : m_data(data)
    {}

    _AtomicNativeNumber(const _AtomicNativeNumber& atom)
    {
      sync::MutexLocker l(m_mutex);
      m_data = atom.m_data;
    }

    // This may cause deadlock, use with caution.
    _AtomicNativeNumber& operator=(const T data)
    {
      sync::MutexLocker l(m_mutex);
      m_data = data;
      return *this;
    }

    _AtomicNativeNumber& operator=(const _AtomicNativeNumber& other)
    {
        if (this != &other)
        {
            T tmp = other;
            sync::MutexLocker l(m_mutex);
            m_data = tmp;
        }
        return *this;
    }

    T operator++()
    {
      sync::MutexLocker l(m_mutex);
      return ++m_data;
    }

    T operator++(int)
    {
      sync::MutexLocker l(m_mutex);
      return m_data++;
    }

    T operator--()
    {
      sync::MutexLocker l(m_mutex);
      return --m_data;
    }

    T operator--(int)
    {
      sync::MutexLocker l(m_mutex);
      return m_data--;
    }

    operator T() const
    {
      sync::MutexLocker l(m_mutex);
      return m_data;
    }

    bool CompareAndSet(const T compare, const T newValue)
    {
        sync::MutexLocker l(m_mutex);
        if (m_data == compare)
        {
            m_data = newValue;
            return true;
        }
        else
        {
            return false;
        }
    }

    T GetData() const
    {
        sync::MutexLocker l(m_mutex);
        return m_data;
    }

    T GetRawData() const
    {
        return m_data;
    }

  private:
    mutable sync::Mutex m_mutex;
    T m_data;
  };

#define DEFINE_ATOMIC_NUM(type) \
  template<> \
  class Atomic<type> : public _AtomicNativeNumber<type> \
  { \
  public: \
    explicit Atomic(const type data) \
    : _AtomicNativeNumber<type>(data) \
    {} \
    \
    Atomic(const Atomic& other) \
    : _AtomicNativeNumber<type>(other) \
    {} \
    \
    Atomic& operator=(const type data) \
    { \
      (void) _AtomicNativeNumber<type>::operator=(data); \
      return *this; \
    } \
    \
    Atomic& operator=(const Atomic& other) \
    { \
      (void) _AtomicNativeNumber<type>::operator=(static_cast<type>(other)); \
      return *this; \
    } \
    \
    bool Equal(const Atomic& other) const \
    { \
      return this == &other || GetData() == other.GetData(); \
    } \
    \
    bool Equal(const type other) const \
    { \
      return GetData() == other; \
    } \
  }

  DEFINE_ATOMIC_NUM(char);
  DEFINE_ATOMIC_NUM(unsigned char);
  DEFINE_ATOMIC_NUM(short);
  DEFINE_ATOMIC_NUM(unsigned short);
  DEFINE_ATOMIC_NUM(int);
  DEFINE_ATOMIC_NUM(unsigned int);
  DEFINE_ATOMIC_NUM(long);
  DEFINE_ATOMIC_NUM(unsigned long);
  DEFINE_ATOMIC_NUM(long long);
  DEFINE_ATOMIC_NUM(unsigned long long);
  

  // Implementation
  template<typename T>
  Atomic<T>::Atomic(ParamType i)
    : m_data(i)
  {
#ifdef _DEBUG_
    std::cout << "Atomic(const T& i)" << std::endl;
#endif
  }

  template<typename T>
  Atomic<T>::Atomic(const Atomic<T>& atom)
    : m_data(atom)
  {
#ifdef _DEBUG_
    std::cout << "Atomic(const Atomic<T>& atom)" << std::endl;
#endif
  }

  template<typename T>
  Atomic<T>& Atomic<T>::operator=(ParamType i)
  {
#ifdef _DEBUG_
    std::cout << "Atomic operator=(const T& i)" << std::endl;
#endif
    sync::MutexLocker l(m_mutex);
    m_data = i;
    return *this;
  }

  template<typename T>
  Atomic<T>& Atomic<T>::operator=(const Atomic<T>& atom)
  {
#ifdef _DEBUG_
    std::cout << "Atomic operator=(const Atomic<T>& atom)" << std::endl;
#endif    
    if (this != &atom)
      {
        T tmp = atom.GetData();
        sync::MutexLocker l(m_mutex);
        m_data = tmp;
      }
    return *this;
  }

  template <typename T>
  bool Atomic<T>::CompareAndSet(ParamType compare, ParamType newValue)
  {
    sync::MutexLocker l(m_mutex);
    if (m_data == compare)
      {
	m_data = newValue;
	return true;
      }
    return false;
  }
  
  template<typename T>
  Atomic<T>::operator T() const
  {
    return GetData();
  }

  template <typename T>
  typename Atomic<T>::RetType Atomic<T>::GetData() const
  {
    sync::MutexLocker l(m_mutex);
    return m_data;
  }

  template <typename T>
  typename Atomic<T>::RetType Atomic<T>::GetRawData() const
  {
    return m_data;
  }

  template <typename T>
  bool Atomic<T>::Equal(const Atomic& other) const
  {
      if (this == &other)
      {
          return true;
      }
      T tmp = static_cast<T>(other);
      sync::MutexLocker l(m_mutex);
      return m_data == tmp;
  }

  template <typename T>
  bool Atomic<T>::Equal(ParamType other) const
  {
      sync::MutexLocker l(m_mutex);
      return m_data == other;
  }
}  // namespace tpool

template<typename T>
inline bool operator==(const ::tpool::Atomic<T>& lhs,
                       const ::tpool::Atomic<T>& rhs)
{
    return lhs.Equal(rhs);
}

template<typename T1, typename T2>
inline bool operator==(const ::tpool::Atomic<T1>& lhs,
                       const T2& rhs)
{
    return lhs.Equal(rhs);
}

template<typename T1, typename T2>
inline bool operator==(const T1& lhs,
                       const ::tpool::Atomic<T2>& rhs)
{
    return rhs.Equal(lhs);
}

#endif
