// -*- mode: c++ -*-
#ifndef _GET_DATA_CHUNK_SERVER_SOCKET_H_
#define _GET_DATA_CHUNK_SERVER_SOCKET_H_

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>


class Socket
{
  typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketImplPtr;
  
public:
  Socket(SocketImplPtr socket);
  virtual ~Socket();

  
  
private:
  SocketImplPtr m_socket;
};

#endif
