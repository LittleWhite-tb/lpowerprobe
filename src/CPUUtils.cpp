#include "CPUUtils.hpp"

#include <cstdio>
#include <errno.h>
#include <cassert>

#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

void CPUUtils::pinCPU(int cpuID)
{
   cpu_set_t cpuset;
   pid_t myself = getpid();

   CPU_ZERO(&cpuset);
   CPU_SET(cpuID,&cpuset);

   int ret = sched_setaffinity(myself,sizeof(cpu_set_t),&cpuset);
   if(ret != 0)
   {
      perror("sched_setaffinity");
      return;
   }
}

void CPUUtils::setFifoMaxPriority(int offset)
{
   struct sched_param sp;
   
   assert(offset < 0);
   
   sched_getparam(0, &sp);
   sp.sched_priority = sched_get_priority_max(SCHED_FIFO) + offset;
   if (sched_setscheduler(0, SCHED_FIFO, &sp))
   {
      perror("setscheduler background");
   }
}
