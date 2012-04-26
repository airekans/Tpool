#ifndef _TPOOL_ATOMIC_H_
#define _TPOOL_ATOMIC_H_

#include "Mutex.h"

namespace tpool {
  template<typename T>
  class Atomic {
  public:
    explicit Atomic(const T& i);
    Atomic(const Atomic& atom);

    Atomic& operator=(const T& i);
    Atomic& operator=(const Atomic& atom);
    operator T() const;
  private:
    T m_data;
    mutable sync::Mutex m_mutex;
  };

  // Implementation
  template<typename T>
  Atomic<T>::Atomic(const T& i)
    : m_data(i)
  {}

  template<typename T>
  Atomic<T>::Atomic(const Atomic<T>& atom)
    : m_data(atom)
  {}

  template<typename T>
  Atomic<T>& Atomic<T>::operator=(const T& i)
  {
    sync::MutexLocker l(m_mutex);
    m_data = i;
    return *this;
  }

  template<typename T>
  Atomic<T>& Atomic<T>::operator=(const Atomic<T>& atom)
  {
    if (this != &atom)
      {
	sync::MutexLocker l(m_mutex);
	m_data = atom;
      }
    return *this;
  }

  template<typename T>
  Atomic<T>::operator T() const
  {
    sync::MutexLocker l(m_mutex);
    return m_data;
  }
  
}

#endif
