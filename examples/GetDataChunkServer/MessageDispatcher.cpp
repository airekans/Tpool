#include "MessageDispatcher.h"
#include <iostream>

using namespace std;
using namespace google::protobuf;

void MessageDispatcher::DefaultHandler(Message* message) const
{
  using simple::GetSimpleDataChunkRequest;
    
  cout << "default handler" << endl;
  GetSimpleDataChunkRequest* request =
    dynamic_cast<GetSimpleDataChunkRequest*>(message);
  if (request != NULL)
    {
      cout << "Request: " << request->num() << endl;
    }
}

