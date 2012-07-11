// -*- mode: c++ -*-
#include "FixedThreadPool.h"
#include <libssh/libsshpp.hpp>
#include <libssh/callbacks.h>
#include <iostream>
#include <cstdlib>
#include <boost/shared_ptr.hpp>
#include <string>
#include <fstream>
#include <arpa/inet.h>
#include <cstdio>
#include <sstream>

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
      if (session->userauthAutopubkey() == SSH_AUTH_SUCCESS)
	{
	  cout << "login successfully" << endl;
	  return;
	}
      cout << "public key auth failed" << endl;
    }
  catch (SshException& e)
    {
      ReportSshError(e);
    }
  
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
      const int count = execChannel->write(realCommand.c_str(), realCommand.size());
      if (count < realCommand.size())
	{
	  cerr << "[error] shell write" << endl;
	  exit(1);
	}
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

int GetServerPort(shared_ptr<Channel> channel)
{
  char execResult[1024] = {0};
  int count = 0;
  int totalCount = 0;
  int port = 0;
  if (!(channel->isOpen()) || channel->isEof())
    {
      cerr << "[Error] command channel not open" << endl;
      exit(1);
    }
  while (true)
    {
      count = channel->read(execResult + totalCount, 1024);
      totalCount += count;
      string s(execResult, totalCount);
      size_t pos = 0;
      if ((pos = s.find("port:")) != string::npos)
	{
	  istringstream iss(s.substr(pos + 5));
	  iss >> port;
	  break;
	}
    }

  string resultString(execResult, totalCount);
  cout << "exec result: " << resultString << endl;
  return port;
}


static int auth_callback(const char *prompt, char *buf, size_t len,
    int echo, int verify, void *userdata)
{
  char *answer = NULL;
  char *ptr;

  (void) verify;
  (void) userdata;

  if (echo) {
    while ((answer = fgets(buf, len, stdin)) == NULL);
    if ((ptr = strchr(buf, '\n'))) {
      *ptr = '\0';
    }
  } else {
    if (ssh_getpass(prompt, buf, len, 0, 0) < 0) {
        return -1;
    }
    return 0;
  }

  if (answer == NULL) {
    return -1;
  }

  strncpy(buf, answer, len);

  return 0;
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

  struct ssh_callbacks_struct cb;
  cb.auth_function = auth_callback;
  cb.userdata = NULL;
  
  ssh_callbacks_init(&cb);
  ssh_set_callbacks(sshSession->getCSession(),&cb);
  
  LoginSshSession(sshSession, username, password);
  
  // execute a command
  string remoteCmd;
  getline(cin, remoteCmd);
  shared_ptr<Channel> execChannel(MakeCommandChannel(sshSession, remoteCmd));
  const int port = GetServerPort(execChannel);


  // open a new forward channel
  shared_ptr<Channel> forwardChannel(MakeDirectForwardChannel(sshSession,
							      "127.0.0.1",
							      port));

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

  execChannel->sendEof();
  execChannel->close();

  
  return 0;
}
