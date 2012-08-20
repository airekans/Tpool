// -*- mode: c++ -*-
#ifndef _GET_DATA_CHUNK_SERVER_MESSAGE_READER_H_
#define _GET_DATA_CHUNK_SERVER_MESSAGE_READER_H_

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include "Socket.h"

class MessageReader {
  
public:
  MessageReader(Socket& socket)
    : m_socket(socket)
  {}

  void Loop();
  
private:
  void ProcessMessagePackage(const long packageLength);
  
  Socket& m_socket;
};


#endif
