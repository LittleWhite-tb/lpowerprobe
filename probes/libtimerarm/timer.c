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

#include "timer.h"

#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#include "threadData.h"
#include "threadFct.h"
#include "barrier.h"


typedef struct sData {
   Barrier barrier;
   pthread_t* pThreads; /*!< IDs of the thread (allow to send signal to these) */
   TData* pThreadsData;
   double* pCycles;     /*!< Data to be send back to lPowerProbe */
   
   unsigned int nbCores;
   char threadMeasureStopper; /*!< Stopper for the measure loop in thread */
   char threadStopper;        /*!< Stopper for the measure thread */
}SData;

extern unsigned int nbDevices (void *data) {
   (void) data;
   return 1;
}

extern unsigned int nbChannels (void *data)
{
    SData* pData = (SData*)data;
    /**
     * If this function has been called once
     * the result is cached. Thus, we don't call
     * multiple times sysconf and display repetitively the error
     */
    if ( pData->nbCores == 0 )
    {
        pData->nbCores = sysconf(_SC_NPROCESSORS_ONLN);
        if (pData->nbCores == 0 )
        {
            fprintf(stderr,"[TIMERANDROID] Fail to get number of core");
            pData->nbCores = 1;
        }
    }
    return pData->nbCores;
}

int initData(SData* pData)
{
   pData->pThreads = NULL;
   pData->pThreadsData = NULL;
   pData->nbCores = 0;
   pData->threadStopper = 0;
   nbChannels(pData);
   
   pData->pThreads = calloc(pData->nbCores,sizeof(*pData->pThreads));
   pData->pThreadsData = calloc(pData->nbCores,sizeof(*pData->pThreadsData));
   pData->pCycles = calloc(pData->nbCores,sizeof(*pData->pCycles));
   if ( pData->pThreads == NULL || pData->pThreadsData == NULL || pData->pCycles == NULL)
   {
      fprintf(stderr,"[TIMERANDROID] Fail to allocate memory for core data");
      return -1;
   }
   
   return 0;
}

void cleanup(SData* pData)
{
   if (pData)
   {
      free(pData->pCycles);
      free(pData->pThreadsData);
      free(pData->pThreads);
   }
   free(pData);
}

void *init (void)
{
   int error = 0;
   SData* pData = calloc(1,sizeof(*pData));
   if ( pData == NULL )
   {
      fprintf(stderr,"[TIMERANDROID] Fail to allocate internal structure\n");
      return NULL;
   }
   
   error = initData(pData);
   if ( error != 0 )
   {
      cleanup(pData);
      return NULL;
   }
      
   error = barrier_create(&pData->barrier,pData->nbCores+1);
   if ( error != 0 )
   {
      fprintf(stderr,"[TIMERANDROID] Fail to create the barrier '%s'\n",strerror(errno));
      cleanup(pData);
      return NULL;
   }
   
   unsigned int i = 0;
   for (i = 0 ; i < pData->nbCores ; i++ )
   {
      // Copy data to be read by the threads
      pData->pThreadsData[i].pBarrier = &pData->barrier;
      
      pData->pThreadsData[i].pThreadMeasureStopper = &pData->threadMeasureStopper;
      pData->pThreadsData[i].pThreadStopper = &pData->threadStopper;
      
      pData->pThreadsData[i].coreID = i;
      if ( pthread_create(&pData->pThreads[i],NULL,measureThread,(void*)&pData->pThreadsData[i]) != 0 )
      {
          fprintf(stderr,"[TIMERANDROID] Failed to create runner thread '%s'\n",strerror(errno));
          cleanup(pData);
          return NULL;
      }
   }
   
   return pData;
}

void fini (void *data)
{
   SData* pData = (SData*)data;
   pData->threadMeasureStopper = 1;
   pData->threadStopper = 1;
   
   if ( barrier_destroy(&pData->barrier) )
   {
      fprintf(stderr,"[TIMERANDROID] Failed to destroy barrier in main thread '%s'\n",strerror(errno));
   }
   
   cleanup(pData);
}

void wakeUpThreads(SData* pData)
{
   unsigned int i = 0;
   
   for ( i = 0 ; i < pData->nbCores ; i++ )
   {
      pthread_kill(pData->pThreads[i],SIGUSR2);
   }
}

void start (void *data)
{
   SData* pData = (SData*)data;
   unsigned int i = 0;
   
   for ( i = 0 ; i < pData->nbCores ; i++ )
   {
      pData->pThreadsData[i].cycles = 0;
   }
   pData->threadMeasureStopper = 0;
   
   if ( barrier_wait(&pData->barrier) != 0 )
   {
      fprintf(stderr,"[TIMERANDROID] Failed to wait barrier in main thread '%s'\n",strerror(errno));
   }
}

double* stop (void *data)
{
   SData* pData = (SData*)data;
   unsigned int i = 0;
   
   pData->threadMeasureStopper = 1;
   wakeUpThreads(pData);
   
   if ( barrier_wait(&pData->barrier) != 0 )
   {
      fprintf(stderr,"[TIMERANDROID] Failed to wait barrier in main thread '%s'\n",strerror(errno));
   }
   
   for ( i = 0 ; i < pData->nbCores ; i++ )
   {
      pData->pCycles[i] = pData->pThreadsData[i].cycles;
   }
   return (double*)pData->pCycles;
}

void update (void *data)
{
   (void) data;
}
