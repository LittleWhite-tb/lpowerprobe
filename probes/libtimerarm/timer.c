/*
Copyright (C) 2014 Exascale Research Center

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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <assert.h>

#include "timer.h"

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

void pinCPU(int cpuID)
{
   cpu_set_t cpuset;
   CPU_ZERO(&cpuset);
   CPU_SET(cpuID,&cpuset);

   pid_t myself = syscall (__NR_gettid);

   int ret = syscall(__NR_sched_setaffinity, myself, sizeof(cpuset), &cpuset);
   if(ret != 0)
   {
      perror("sched_setaffinity");
      return;
   }
}

#if defined(__arm__)
   #define PERF_DEF_OPTS (1 | 16)

   static __inline__ unsigned long long rdtsc(void)
   {
      uint32_t r = 0;
      asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(r) );
      return r;
   }

   #define rdtscll(val) { val = rdtsc(); }
#else
    #error "This library 'timerarm' is only for ARM target"
#endif

#ifndef _SC_NPROCESSORS_ONLN
    #error "Number of CPU unavailable"
#endif

const unsigned int version = LPP_API_VERSION;
const char *label = "Cycles";
const unsigned int period = 0;

struct sThreadData;

/**
 * Global struct
 */
typedef struct sData {
   unsigned int nbCores;
   double* pCyclesMeasures;
   double* pCyclesDelta;

   pthread_t* pThreadsId;
   struct sThreadData* pThreadData;
   char threadStopper;

   pthread_mutex_t masterSyncMutex;
   pthread_cond_t masterSyncCond;
   pthread_mutex_t measureSyncMutex;
   pthread_cond_t measureSyncCond;
   pthread_mutex_t startSyncMutex;
   pthread_cond_t startSyncCond;
   unsigned int measureCounter;
   unsigned int readyCounter;
} SData;

/**
 * Structure passed to the thread
 */
typedef struct sThreadData {
    int threadId;
    SData* pData;
}SThreadData;

static __inline__ unsigned long long getticks(void)
{
   unsigned long long ret;
   rdtscll(ret);
   return ret;
}

void* measureThread(void* pThreadData)
{
    SThreadData* pTData = (SThreadData*)pThreadData;
    assert(pTData);

    pinCPU(pTData->threadId);

    while(pTData->pData->threadStopper == 0)
    {
        // Tell the master thread that we finished
        if ( pTData->pData->threadStopper == 0 )
        {
            pthread_mutex_lock(&pTData->pData->startSyncMutex);
        }
        else
        {
            break;
        }

        pTData->pData->readyCounter++;
        if ( pTData->pData->readyCounter == pTData->pData->nbCores )
        {
            pthread_cond_signal(&pTData->pData->startSyncCond);
        }

        // Wait for a measure to be started
        if (pTData->pData->threadStopper == 0)
        {
            pthread_mutex_lock(&pTData->pData->measureSyncMutex);
        }

        pthread_mutex_unlock(&pTData->pData->startSyncMutex);
        if (pTData->pData->threadStopper == 1)
        {
            pthread_mutex_unlock(&pTData->pData->measureSyncMutex);
            break;
        }

        pthread_cond_wait(&(pTData->pData->measureSyncCond),&(pTData->pData->measureSyncMutex));

        pthread_mutex_unlock(&pTData->pData->measureSyncMutex);
        if ( pTData->pData->threadStopper == 1 )
        {
            break;
        }

        uint32_t value = 0;
        asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(value));
        pTData->pData->pCyclesMeasures[pTData->threadId] = value;


        // Tell the master thread that we finished
        if ( pTData->pData->threadStopper == 0 )
        {
            pthread_mutex_lock(&pTData->pData->masterSyncMutex);
        }
        else
        {
            break;
        }

        pTData->pData->measureCounter++;
        if ( pTData->pData->measureCounter >= pTData->pData->nbCores )
        {
            // Throw signal only if needed
            pthread_cond_signal(&pTData->pData->masterSyncCond);
        }
        pthread_mutex_unlock(&pTData->pData->masterSyncMutex);
    }

    return NULL;
}

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

void cleanup(SData* pData)
{
    if ( pData != NULL )
    {
        pData->threadStopper = 1;
        if ( pData->pThreadData != NULL )
        {
            pthread_mutex_lock(&pData->measureSyncMutex);
            pthread_cond_broadcast(&pData->measureSyncCond);
            pthread_mutex_unlock(&pData->measureSyncMutex);

            unsigned int thread = 0;
            for (thread = 0 ; thread < pData->nbCores ;
                 thread++)
            {
                pthread_join(pData->pThreadsId[thread],NULL);
            }
        }

        pthread_cond_destroy(&pData->masterSyncCond);
        pthread_mutex_destroy(&pData->masterSyncMutex);
        pthread_cond_destroy(&pData->startSyncCond);
        pthread_mutex_destroy(&pData->startSyncMutex);
        pthread_cond_destroy(&pData->measureSyncCond);
        pthread_mutex_destroy(&pData->measureSyncMutex);

        free(pData->pThreadsId);
        free(pData->pCyclesMeasures);
        free(pData->pCyclesDelta);
        free(pData->pThreadData);
        free(pData);
        pData=NULL;
    }
}

extern void *init (void)
{
   SData *pData = malloc (sizeof (*pData));
   if ( pData == NULL )
   {
       fprintf(stderr,"Failed to allocate memory (pData)\n");
       return NULL;
   }

   // Init inner fields
   pData->threadStopper = 0;
   pData->readyCounter = 0;
   pData->nbCores = 0;
   nbChannels(pData);

   pData->pThreadsId = malloc(sizeof(*pData->pThreadsId) * pData->nbCores);
   pData->pCyclesMeasures = malloc(sizeof(*pData->pCyclesMeasures) * pData->nbCores);
   pData->pCyclesDelta = malloc(sizeof(*pData->pCyclesDelta) * pData->nbCores);
   pData->pThreadData = malloc(sizeof(*pData->pThreadData) * pData->nbCores);
   if ( pData->pThreadsId == NULL || pData->pCyclesMeasures == NULL || pData->pCyclesDelta == NULL || pData->pThreadData  == NULL )
   {
       fprintf(stderr,"Failed to allocate memory (%p | %p | %p | %p)\n",pData->pThreadsId,pData->pCyclesMeasures,pData->pCyclesDelta,pData->pThreadData);
       cleanup(pData);
       return NULL;
   }

   if ( pthread_mutex_init(&pData->masterSyncMutex, NULL) != 0 )
   {
       fprintf(stderr,"Failed to init mutex\n");
       cleanup(pData);
       return NULL;
   }
   if ( pthread_cond_init(&pData->masterSyncCond, NULL) != 0 )
   {
       fprintf(stderr,"Failed to init condition\n");
       cleanup(pData);
       return NULL;
   }
   if ( pthread_mutex_init(&pData->startSyncMutex, NULL) != 0 )
   {
       fprintf(stderr,"Failed to init mutex\n");
       cleanup(pData);
       return NULL;
   }
   if ( pthread_cond_init(&pData->startSyncCond, NULL) != 0 )
   {
       fprintf(stderr,"Failed to init condition\n");
       cleanup(pData);
       return NULL;
   }
   if ( pthread_mutex_init(&pData->measureSyncMutex, NULL) != 0 )
   {
       fprintf(stderr,"Failed to init mutex\n");
       cleanup(pData);
       return NULL;
   }
   if ( pthread_cond_init(&pData->measureSyncCond, NULL) != 0 )
   {
       fprintf(stderr,"Failed to init condition\n");
       cleanup(pData);
       return NULL;
   }

   unsigned int thread = 0;
   for (thread = 0 ; thread < pData->nbCores ;
        thread++)
   {
       pthread_attr_t attr;
       pData->pThreadData[thread].pData= pData;
       pData->pThreadData[thread].threadId = thread;

       if (pthread_attr_init(&attr) != 0 )
       {
           fprintf(stderr,"Failed to init attributes for thread\n");
       }
       if ( pthread_create(&pData->pThreadsId[thread],&attr,measureThread,(void*) &pData->pThreadData[thread]) != 0 )
       {
           fprintf(stderr,"Failed to create runner thread\n");
       }

       pthread_attr_destroy(&attr);
   }



   return pData;
}

void startMeasure(SData* pData)
{
    assert(pData);

    pthread_mutex_lock(&pData->startSyncMutex);

    while (pData->readyCounter < pData->nbCores)
    {
        pthread_cond_wait(&pData->startSyncCond,&pData->startSyncMutex);
    }
    pData->readyCounter = 0;

    pthread_mutex_unlock(&pData->startSyncMutex);

    // Now, we have to wait for the measure

    pthread_mutex_lock(&pData->masterSyncMutex);
    pData->measureCounter = 0;

    pthread_mutex_lock(&pData->measureSyncMutex);
    pthread_cond_broadcast(&pData->measureSyncCond);
    pthread_mutex_unlock(&pData->measureSyncMutex);

    while (pData->measureCounter < pData->nbCores)
    {
        pthread_cond_wait(&pData->masterSyncCond,&pData->masterSyncMutex);
    }
    pthread_mutex_unlock(&pData->masterSyncMutex);
}

extern void start (void *data)
{
    if ( data != NULL )
    {
        SData *pData = (SData*) data;
        startMeasure(pData);

        memcpy(pData->pCyclesDelta,pData->pCyclesMeasures,sizeof(*pData->pCyclesMeasures) * pData->nbCores);
    }
}

extern double *stop (void *data)
{
    if ( data != NULL )
    {
        SData *pData = (SData*) data;
        startMeasure(pData);

        unsigned int i = 0;
        for ( i = 0 ; i < pData->nbCores ; i++ )
        {
            pData->pCyclesDelta[i] = pData->pCyclesMeasures[i] - pData->pCyclesDelta[i];
        }
        return pData->pCyclesDelta;
    }
    return NULL;
}

extern void fini (void *data) {
   cleanup((SData*)data);
}

extern void update (void *data) {
    if ( data != NULL )
    {
        SData *pData = (SData*) data;
        startMeasure(pData);

        unsigned int i = 0;
        for ( i = 0 ; i < pData->nbCores ; i++ )
        {
            pData->pCyclesDelta[i] = pData->pCyclesMeasures[i] - pData->pCyclesDelta[i];
        }
    }
}
