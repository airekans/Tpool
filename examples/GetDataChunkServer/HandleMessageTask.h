// -*- mode: c++ -*-
#ifndef _GET_DATA_CHUNK_SERVER_HANDLE_MESSAGE_TASK_H_
#define _GET_DATA_CHUNK_SERVER_HANDLE_MESSAGE_TASK_H_

#include "TaskBase.h"
#include "MessageHandler.h"
#include "Socket.h"


class HandleMessageTask : public tpool::TaskBase
{
public:
  HandleMessageTask(MessageHandler::Ptr handler,
		    google::protobuf::Message* message,
		    Socket& socket);
  virtual ~HandleMessageTask();

  virtual void Run();

private:
  MessageHandler::Ptr m_handler;
  google::protobuf::Message* m_message;
  Socket& m_socket;
};

#endif

