// -*- mode: c++ -*-
#include "FixedThreadPool.h"
#include <libssh/libsshpp.hpp>
#include <iostream>
#include <cstdlib>
#include <boost/shared_ptr.hpp>
#include <string>

using namespace tpool;
using namespace std;
using namespace ssh;
using boost::shared_ptr;

class SshForwardTask : public TaskBase
{
  shared_ptr<Channel> m_forwardChannel;
  sync::Mutex& m_channelGuard;
  
public:
  SshForwardTask(shared_ptr<Channel> channel, sync::Mutex& channelGuard)
    : m_forwardChannel(channel), m_channelGuard(channelGuard)
  {}
  
  virtual ~SshForwardTask()
  {}

  virtual void DoRun()
  {
    const int data = 1;
    sync::MutexLocker l(m_channelGuard);
    cout << "forward data: " << data << endl;
    m_forwardChannel->write(&data, sizeof(data));
  }
};

void ReportSshError(SshException& e)
{
  cerr << "[Error]: " << e.getError() << endl;
  exit(1);
}


int main(int argc, char *argv[])
{
  if (argc < 3)
    {
      cerr << "please enter username and password" << endl;
      exit(1);
    }
  const string username = argv[1];
  const string password = argv[2];
  
  Session sshSession;
  sshSession.setOption(SSH_OPTIONS_HOST, "127.0.0.1");
  sshSession.setOption(SSH_OPTIONS_PORT, 22);
  try
    {
      sshSession.connect();
    }
  catch (SshException& e)
    {
      ReportSshError(e);
    }

  sshSession.setOption(SSH_OPTIONS_USER, username.c_str());
  try
    {
      if (sshSession.userauthPassword(password.c_str()) != SSH_AUTH_SUCCESS)
	{
	  cerr << "[Error] login failed" << endl;
	  exit(1);
	}
      cout << "login successfully" << endl;
    }
  catch (SshException& e)
    {
      ReportSshError(e);
    }

  
  // execute a command
  Channel execChannel(sshSession);
  try
    {
      execChannel.openSession();
      execChannel.requestExec("ls /home");
    }
  catch (SshException& e)
    {
      ReportSshError(e);
    }
  char execResult[1024] = {0};
  int count = 0;
  if ((count = execChannel.read(execResult, 1024)) < 0)
    {
      cerr << "[Error] read" << endl;
      exit(1);
    }
  cout << "exec result: " << string(execResult, count) << endl;
  execChannel.close();

  shared_ptr<Channel> forwardChannel(new Channel(sshSession));
  try
    {
      forwardChannel->openForward("127.0.0.1", 8999, "localhost");
    }
  catch (SshException& e)
    {
      ReportSshError(e);
    }

  {
    LFixedThreadPool threadPool(5);
    sync::Mutex channelGuard;

    for (int i = 0; i < 5; ++i)
      {
	threadPool.AddTask(TaskBase::Ptr(new SshForwardTask(forwardChannel, channelGuard)));
      }

    threadPool.Stop();
  }

  return 0;
}
