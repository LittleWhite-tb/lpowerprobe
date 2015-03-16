/*
Copyright (C) 2015 Exascale Research Center

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "threadData.h"

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>

void pinCPU(int cpuID)
{
   int mask = 0x00000001;
   while (cpuID--)
   {
      mask = mask << 1;
   }

   pid_t myself = syscall (__NR_gettid);
   
   fprintf(stderr,"[TIMERANDROID] Set thread %d pin on %X\n",myself,mask);

   int ret = syscall(__NR_sched_setaffinity, myself, sizeof(mask), &mask);
   if(ret != 0)
   {
      perror("sched_setaffinity");
      return;
   }
}

inline uint32_t getNbCycles()
{
   uint32_t value = 0;
   asm volatile("MRC p15, 0, %0, c9, c13, 0" : "=r"(value));
   return value;
}

inline unsigned int hasOverflow()
{
   uint32_t overflowFlag = 0;
   asm volatile("MRC p15, 0, %0, c9, c12, 3\t\n" : "=r"(overflowFlag));
   if ((overflowFlag & 0x80000000) == 0x80000000)
   {
      return 1;
   }
   
   return 0;
}

inline void resetCycleCounter()
{
   asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(1|4));  
}

inline void resetOverflow()
{
   asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000000f));
}

void dummySignalHandle()
{
}

void* measureThread(void* pThreadData)
{
   TData* pData = (TData*)pThreadData;
   int error = 0;
   
   fprintf(stderr,"[TIMERANDROID-THREAD%d] Started\n",pData->coreID);
   
   pinCPU(pData->coreID);
   signal(SIGUSR2, dummySignalHandle);
   
   while (*(pData->pThreadStopper) == 0 )
   {
         
      
      fprintf(stderr,"[TIMERANDROID-THREAD%d] Wait for start\n",pData->coreID);
      error = barrier_wait(pData->pBarrier);
      if ( error != 0 )
      {
         fprintf(stderr,"[TIMERANDROID] Failed to wait barrier in measure thread '%s'\n",strerror(errno));
         return NULL;
      }
      
      // Get actual value
      resetCycleCounter();
      while(*(pData->pThreadMeasureStopper) == 0)
      {
         const struct timespec sec = { 1 , 0 };
         fprintf(stderr,"[TIMERANDROID-THREAD%d] Will sleep\n",pData->coreID);
         int result = nanosleep(&sec,NULL);
         fprintf(stderr,"[TIMERANDROID-THREAD%d] Woke up\n",pData->coreID);
         if ( result == -1 )
         {
            fprintf(stderr,"Stopped by signal on thread %d\n",pData->coreID);
         }
         
         if ( hasOverflow() == 1)
         {
            pData->cycles += UINT_MAX;
            resetOverflow();
         }
         pData->cycles += getNbCycles();
         resetCycleCounter();
      }
      
      fprintf(stderr,"[TIMERANDROID-THREAD%d] Wait for stop (actual value : %llu)\n",pData->coreID,pData->cycles);
      if ( barrier_wait(pData->pBarrier) )
      {
         fprintf(stderr,"[TIMERANDROID] Failed to wait barrier in main thread '%s'\n",strerror(errno));
      }
   }
      
   
   fprintf(stderr,"[TIMERANDROID-THREAD%d] Finished\n",pData->coreID);
   return NULL;
}
