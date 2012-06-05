// -*- mode: c++ -*-
#ifndef _GET_DATA_CHUNK_SERVER_MESSAGE_READER_H_
#define _GET_DATA_CHUNK_SERVER_MESSAGE_READER_H_

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

class MessageReader {
  
public:
  MessageReader(boost::shared_ptr<boost::asio::ip::tcp::socket> socket)
    : m_socket(socket)
  {}

  void Loop();
  
private:
  void ProcessMessagePackage(const long packageLength);
  
  boost::shared_ptr<boost::asio::ip::tcp::socket> m_socket;
};


#endif
