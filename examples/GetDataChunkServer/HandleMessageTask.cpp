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

void HandleMessageTask::DoRun()
{
  using boost::shared_ptr;

  shared_ptr<Message> response(m_handler->HandleMessage(m_message));
  if (response)
    {
        ostringstream oss;
	response->SerializeToOstream(&oss);
	cout << "response length: " << oss.str().length() << endl;

	m_socket.Write(oss.str());
    }
}
