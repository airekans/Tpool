#include "Socket.h"

using namespace std;


Socket::Socket(SocketImplPtr socket)
  : m_socket(socket)
{}

Socket::~Socket()
{}

Socket::SocketImplPtr Socket::GetImpl() const
{
  return m_socket;
}

void Socket::Write(const string& buffer)
{
  boost::system::error_code ignored_error;
  boost::asio::write(*m_socket, boost::asio::buffer(buffer),
		     boost::asio::transfer_all(), ignored_error);
}

