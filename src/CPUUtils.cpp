/*
 * lPowerProbe - A light benchmark tool oriented for energy probing
 *               heavely based on likwid
 * Copyright (C) 2013-2014 Universite de Versailles
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
#include <sys/syscall.h>
#include <unistd.h>

#ifdef ANDROID_PLATFORM   
   // On Android < 4.2, these are not set
   #define CPU_SETSIZE   32
   #define __CPU_BITTYPE    unsigned long int  /* mandated by the kernel  */
   #define __CPU_BITSHIFT   5                  /* should be log2(BITTYPE) */
   #define __CPU_BITS       (1 << __CPU_BITSHIFT)
   #define __CPU_ELT(x)     ((x) >> __CPU_BITSHIFT)
   #define __CPU_MASK(x)    ((__CPU_BITTYPE)1 << ((x) & (__CPU_BITS-1)))
   
   typedef struct {
      __CPU_BITTYPE  __bits[ CPU_SETSIZE / __CPU_BITS ];
   } cpu_set_t;
   
   #  define CPU_ZERO(set_)   \
    do{ \
        (set_)->__bits[0] = 0; \
    }while(0)
   
   #  define CPU_SET(cpu_,set_) \
    do {\
        size_t __cpu = (cpu_); \
        if (__cpu < CPU_SETSIZE) \
            (set_)->__bits[0] |= __CPU_MASK(__cpu); \
    }while (0)
#endif

void CPUUtils::pinCPU(int cpuID)
{
   cpu_set_t cpuset;
   CPU_ZERO(&cpuset);
   CPU_SET(cpuID,&cpuset);
   
   pid_t myself = syscall (__NR_gettid);
   
#ifdef ANDROID_PLATFORM
   int ret = syscall(__NR_sched_setaffinity, myself, sizeof(cpuset), &cpuset);
#else
   int ret = sched_setaffinity(myself,sizeof(cpu_set_t),&cpuset);
#endif
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
