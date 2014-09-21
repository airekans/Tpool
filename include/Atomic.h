// -*- mode: c++ -*-
#ifndef _TPOOL_ATOMIC_H_
#define _TPOOL_ATOMIC_H_

#include "Mutex.h"
#ifdef _DEBUG_
#include <iostream>
#endif

namespace tpool {
  template<typename T>
  class Atomic {
  public:
    explicit Atomic(const T& i);
    Atomic(const Atomic& atom);

    Atomic& operator=(const T& i);
    Atomic& operator=(const Atomic& atom);
    bool CompareAndSet(const T& compare, const T& newValue);
    operator T() const;
    const T& GetData() const;
    const T& GetRawData() const;

  private:
    mutable sync::Mutex m_mutex;
    T m_data;
  };

  // Partial specialization
  template <>
  class Atomic<unsigned>
  {
  public:
    explicit Atomic(const unsigned data)
    : m_data(data)
    {}

    Atomic(const Atomic& atom)
    {
      sync::MutexLocker l(m_mutex);
      m_data = atom.m_data;
    }

    // This may cause deadlock, use with caution.
    Atomic& operator=(const unsigned data)
    {
      sync::MutexLocker l(m_mutex);
      m_data = data;
      return *this;
    }

    unsigned operator++()
    {
      sync::MutexLocker l(m_mutex);
      return ++m_data;
    }

    unsigned operator++(int)
    {
      sync::MutexLocker l(m_mutex);
      return m_data++;
    }

    unsigned operator--()
    {
      sync::MutexLocker l(m_mutex);
      return --m_data;
    }

    unsigned operator--(int)
    {
      sync::MutexLocker l(m_mutex);
      return m_data--;
    }

    operator unsigned() const
    {
      sync::MutexLocker l(m_mutex);
      return m_data;
    }

  private:
    mutable sync::Mutex m_mutex;
    unsigned m_data;
  };
  
  // Implementation
  template<typename T>
  Atomic<T>::Atomic(const T& i)
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
  Atomic<T>& Atomic<T>::operator=(const T& i)
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
	sync::MutexLocker l(m_mutex);
	m_data = atom;
      }
    return *this;
  }

  template <typename T>
  bool Atomic<T>::CompareAndSet(const T& compare, const T& newValue)
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
  const T& Atomic<T>::GetData() const
  {
    sync::MutexLocker l(m_mutex);
    return m_data;
  }

  template <typename T>
  const T& Atomic<T>::GetRawData() const
  {
    return m_data;
  }
}

#endif
