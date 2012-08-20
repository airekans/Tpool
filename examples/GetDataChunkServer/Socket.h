// -*- mode: c++ -*-
#ifndef _GET_DATA_CHUNK_SERVER_SOCKET_H_
#define _GET_DATA_CHUNK_SERVER_SOCKET_H_

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <string>
#include "Mutex.h"


class Socket
{
  typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketImplPtr;
  
public:
  Socket(SocketImplPtr socket);
  virtual ~Socket();

  SocketImplPtr GetImpl() const;

  template <typename Buffer>
  std::size_t Read(const Buffer& buffer);

  void Write(const std::string& buffer);
  
private:
  SocketImplPtr m_socket;
  tpool::sync::Mutex m_socketGuard;
};

template <typename Buffer>
std::size_t Socket::Read(const Buffer& buffer)
{
  using tpool::sync::MutexLocker;

  MutexLocker l(m_socketGuard);
  return m_socket->read_some(buffer);
}

#endif
