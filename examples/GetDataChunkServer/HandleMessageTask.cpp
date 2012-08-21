#include "HandleMessageTask.h"
#include <sstream>
#include <iostream>

using namespace google::protobuf;
using namespace std;


HandleMessageTask::HandleMessageTask(MessageHandler::Ptr handler,
				     google::protobuf::Message* message,
				     Socket& socket)
  : m_handler(handler), m_message(message), m_socket(socket)
{}

HandleMessageTask::~HandleMessageTask()
{}

void HandleMessageTask::Run()
{
  Message* response = m_handler->HandleMessage(m_message);
  if (response != NULL)
    {
        ostringstream oss;
	response->SerializeToOstream(&oss);
	cout << "response length: " << oss.str().length() << endl;

	m_socket.Write(oss.str());
    }
}
