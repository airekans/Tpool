#include "EndTask.h"
#include <exception>
#include <string>

using namespace tpool;
using namespace std;

namespace {
  // This exception makes thread exit.
  class ThreadExitException : public exception {
  public:
    explicit ThreadExitException(const string& s)
      : m_exitMessage(s)
    {}

    virtual ~ThreadExitException() throw()
    {}

    virtual const char* what() const throw()
    {
      return m_exitMessage.c_str();
    }

  private:
    string m_exitMessage;
  };
}

void EndTask::Do()
{
  throw ThreadExitException("exit thread");
}
