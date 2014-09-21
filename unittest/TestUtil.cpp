#include "TestUtil.h"

#include <sys/select.h>


void tpool::unittest::MilliSleep(TimeValue time_in_ms)
{
  struct timeval timeout;
  timeout.tv_sec = time_in_ms / 1000;
  timeout.tv_usec = (time_in_ms % 1000) * 1000;
  (void) select(0, NULL, NULL, NULL, &timeout);
}

