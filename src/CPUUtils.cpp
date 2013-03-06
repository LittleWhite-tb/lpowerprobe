/*
 * lPowerProbe - A light benchmark tool oriented for energy probing
 *               heavely based on likwid
 * Copyright (C) 2013 Universite de Versailles
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
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
   //sp.sched_priority = sched_get_priority_max(SCHED_OTHER) + offset;
   sp.sched_priority = sched_get_priority_max(SCHED_OTHER);
   if (sched_setscheduler(0, SCHED_OTHER, &sp))
   {
      perror("setscheduler background");
   }

   nice(-20 - offset);
}
