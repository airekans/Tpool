// -*- mode: c++ -*-
#include "FixedThreadPool.h"
#include <libssh/libsshpp.hpp>
#include <iostream>
#include <cstdlib>
#include <boost/shared_ptr.hpp>
#include <string>
#include <fstream>
#include <arpa/inet.h>
#include <cstdio>

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
    const long networkData = htonl(data);
    m_forwardChannel->write(&networkData, sizeof(data));
  }
};

void ReportSshError(SshException& e)
{
  cerr << "[Error]: " << e.getError() << endl;
  exit(1);
}

shared_ptr<Session> MakeSshSession(const string& host, const int port)
{
  shared_ptr<Session> session(new Session);
  session->setOption(SSH_OPTIONS_HOST, host.c_str());
  session->setOption(SSH_OPTIONS_PORT, port);
  try
    {
      session->connect();
      return session;
    }
  catch (SshException& e)
    {
      ReportSshError(e);
    }
}

void LoginSshSession(shared_ptr<Session> session, const string& username,
		     const string& password)
{
  session->setOption(SSH_OPTIONS_USER, username.c_str());
  try
    {
      if (session->userauthPassword(password.c_str()) != SSH_AUTH_SUCCESS)
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
}

shared_ptr<Channel> MakeCommandChannel(shared_ptr<Session> session,
				       const string& command)
{
  shared_ptr<Channel> execChannel(new Channel(*session));
  try
    {
      execChannel->openSession();
      execChannel->requestPty();
      execChannel->changePtySize(80, 46);
      execChannel->requestShell();
      const string realCommand = command + "\n";
      cout << "realcommand is " << realCommand;
      cout << "size is " << realCommand.size() << endl;
      {
	FILE* logfile = fopen("log.txt", "wb");
	fwrite(realCommand.c_str(), 1, realCommand.size(), logfile);
	fclose(logfile);
      }
      const int count = execChannel->write(realCommand.c_str(), realCommand.size());
      if (count < realCommand.size())
	{
	  cerr << "[error] shell write" << endl;
	  exit(1);
	}
      // execChannel->requestExec(command.c_str());
      return execChannel;
    }
  catch (SshException& e)
    {
      ReportSshError(e);
    }
}

shared_ptr<Channel> MakeDirectForwardChannel(shared_ptr<Session> session,
					     const string& host,
					     const int& port)
{
  shared_ptr<Channel> forwardChannel(new Channel(*session));
  try
    {
      forwardChannel->openForward(host.c_str(), port, "localhost");
      if (!(forwardChannel->isOpen()))
	{
	  cerr << "Forward Channel Open Failed." << endl;
	  exit(1);
	}
      return forwardChannel;
    }
  catch (SshException& e)
    {
      ReportSshError(e);
    }
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
  
  shared_ptr<Session> sshSession(MakeSshSession("127.0.0.1", 22));
  LoginSshSession(sshSession, username, password);
  
  // execute a command
  string remoteCmd;
  getline(cin, remoteCmd);
  {
    cout << "remotecommand is " << remoteCmd << endl;
    shared_ptr<Channel> execChannel(MakeCommandChannel(sshSession, remoteCmd));

    char execResult[1024] = {0};
    int count = 0;
    if (!(execChannel->isOpen()) || execChannel->isEof())
      {
	cerr << "[Error] command channel not open" << endl;
	exit(1);
      }
    // if ((count = execChannel->read(execResult, 1024)) < 0)
    //   {
    // 	cerr << "[Error] read" << endl;
    // 	exit(1);
    //   }
    while ((count = execChannel->read(execResult, sizeof(execResult))) > 0)
      {
	string resultString(execResult, count);
	cout << "exec result: " << resultString << endl;
      }
    string resultString(execResult, count);
    cout << "exec result: " << resultString << endl;
    execChannel->requestSendSignal("KILL");
    execChannel->sendEof();
    execChannel->close();
    cin >> remoteCmd;
  }

  shared_ptr<Channel> forwardChannel(MakeDirectForwardChannel(sshSession,
							      "127.0.0.1",
							      8999));

  {
    LFixedThreadPool threadPool(5);
    sync::Mutex channelGuard;

    for (int i = 0; i < 5; ++i)
      {
	threadPool.AddTask(TaskBase::Ptr(new SshForwardTask(forwardChannel,
							    channelGuard)));
      }

    threadPool.Stop();
  }

  
  return 0;
}
