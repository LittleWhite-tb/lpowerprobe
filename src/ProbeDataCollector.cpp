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

#include "ProbeDataCollector.hpp"

#include <iostream>
#include <exception>
#include <stdexcept>
#include <cassert>
#include <climits>
#include <csignal>

#include <ctime>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <pthread.h>

#include "RunData.hpp"
#include "CPUUtils.hpp"

pid_t gettid()
{
   return syscall(SYS_gettid);
}

static void threadSigalrmHandler(int sig) {
   (void) sig;

   // do nothing here
}

void* probeThread(void* args)
{
    ProbeDataCollector* pProbeDataCollector = reinterpret_cast<ProbeDataCollector*>(args);
    pProbeDataCollector->updateThread();

    return NULL;
}

static int gcd(int val1, int val2)
{
    int ret = 0;

    if (val1 < val2)
        std::swap(val1, val2);

    ret = val1 % val2;

    if (ret == 0)
        return val2;
    else
        return gcd (val2, ret);
}

ProbeDataCollector::ProbeDataCollector(ProbeList* pProbes)
    :m_needThread(false),m_threadCollecting(false),m_threadRunning(true),m_pProbes(pProbes),m_minPeriod(INT_MAX)
{
    assert(pProbes);

    // Get the highest update period required
    for (ProbeList::const_iterator itProbe = m_pProbes->begin() ; itProbe != m_pProbes->end() ; ++itProbe)
    {
        unsigned int period = (*itProbe)->getPeriod();
        if ( period != 0 )
        {
            m_periodicProbes.push_back(*itProbe);
            if ( m_minPeriod != INT_MAX )
            {
                m_minPeriod = gcd(m_minPeriod,period);
            }
            else // First time here
            {
                m_minPeriod = period;
            }
            m_needThread = true;
        }
    }
    std::cout << "Min period : " << m_minPeriod << std::endl;

    // Create the update thread
    if (m_needThread)
    {
        // Create mutex
        pthread_mutexattr_t mattrs;

        if (pthread_mutexattr_init(&mattrs) != 0) {
           std::cerr << "Failed to initialize collector thread mutex" << std::endl;
           throw std::runtime_error("pthread_mutexattr_init");
        }

        if ( pthread_mutex_init(&m_mutex, &mattrs) != 0 )
        {
            std::cerr << "Failed to create mutex for collector thread" << std::endl;
            throw std::runtime_error("pthread_mutex_init");
        }
        
        // Create condition
        if ( pthread_cond_init(&m_cond, NULL) != 0 )
        {
           std::cerr << "Failed to create condition for collector thread" << std::endl;
           throw std::runtime_error("pthread_mutex_init");
        }

        pthread_mutexattr_destroy(&mattrs);

        pthread_attr_t attr;
        if (pthread_attr_init(&attr) != 0 )
        {
            std::cerr << "Failed to init attributes for thread" << std::endl;
            throw std::runtime_error("pthread_attr_init");
        }
         
        // this new thread is not qualified to handle termination and user signals
        sigset_t blocked, oldsigs;
        sigemptyset(&blocked);
        sigaddset(&blocked, SIGTERM);
        sigaddset(&blocked, SIGINT);
        sigaddset(&blocked, SIGUSR1);
        sigaddset(&blocked, SIGUSR2);
        pthread_sigmask(SIG_BLOCK, &blocked, &oldsigs);   // inherited

        if ( pthread_create(&m_collectorThread,&attr,probeThread,this) != 0 )
        {
            std::cerr << "Failed to create collector thread" << std::endl;
            throw std::runtime_error("pthread_create");
        }

        if ( pthread_attr_destroy(&attr) != 0 )
        {
            std::cerr << "Failed to destroy the atrribute thread" << std::endl;
            throw std::runtime_error("pthread_attr_destroy");
        }


        // restore the old signal state
        pthread_sigmask(SIG_SETMASK, &oldsigs, NULL);
    }
}

ProbeDataCollector::~ProbeDataCollector()
{
    if ( m_needThread )
    {
        m_threadRunning = false;
        pthread_cond_signal(&m_cond); // Make the thread unlock
        pthread_kill(m_collectorThread, SIGALRM); // in case it was running
        pthread_join(m_collectorThread, NULL);

        if ( pthread_cond_destroy(&m_cond) != 0 )
        {
            std::cerr << "Failed to destroy cond" << std::endl;
            throw std::runtime_error("pthread_cond_destroy");
        }
        
        if ( pthread_mutex_destroy(&m_mutex) != 0 )
        {
            std::cerr << "Failed to destroy mutex" << std::endl;
            throw std::runtime_error("pthread_mutex_destroy");
        }
    }
}

void ProbeDataCollector::start()
{
    for (ProbeList::const_iterator itProbe = m_pProbes->begin() ; itProbe != m_pProbes->end() ; ++itProbe)
    {
        (*itProbe)->startMeasure();
    }

    if ( m_needThread )
    {
        m_threadCollecting = true;
        pthread_cond_signal(&m_cond); // Unlock thread
    }
}

void ProbeDataCollector::stop(ExperimentationResults* pResults)
{
    if ( pResults->isFull() )
    {
        std::cerr << "Warning : we are allocating more memory for the measurements. This is because the ExperimentationResults is undersized" << std::endl;
        pResults->extend(*m_pProbes);
    }

    for (unsigned int i = 0 ; i < m_pProbes->size() ; i++)
    {
        pResults->setProbeData(i,(*m_pProbes)[i]->stopMeasure());
    }
    pResults->measurementDone();

    // Blocking the update thread
    if ( m_needThread )
    {
        m_threadCollecting = false; // Make thread to wait for next start
        pthread_kill(m_collectorThread, SIGUSR1); // Unlock on sleep
    }
}

void ProbeDataCollector::cancel() 
{
    for (unsigned int i = 0 ; i < m_pProbes->size() ; i++)
    {
        (*m_pProbes)[i]->stopMeasure();
    }

    // Blocking the update thread
    if ( m_needThread )
    {
        m_threadCollecting = false; // Make thread to wait for next start
        pthread_kill(m_collectorThread, SIGALRM);
    }
}

void ProbeDataCollector::updateThread()
{
   long unsigned int elapsedTime = 0; /* in µs */
   timespec ts;
   ts.tv_sec = m_minPeriod/1000000;
   ts.tv_nsec = (m_minPeriod%1000000) *1000;

   // capture SIGALRM signals (and do nothing of them)
   struct sigaction sact;
   sigset_t mask;

   sigemptyset(&mask);
   sact.sa_handler = threadSigalrmHandler;
   sact.sa_mask = mask;
   sact.sa_flags = 0;

   sigaction(SIGALRM, &sact, NULL);

   // main loop
   while ( m_threadRunning )
   {
      if ( !m_threadRunning )
      {
         pthread_cond_wait(&m_cond,&m_mutex);
      }

      if (!m_threadRunning)
      {
          // Unlock the mutex because we just locked it (this unlock allows to destroy it)
         pthread_mutex_unlock(&m_mutex);
         break;
      }

      // do not update if the sleep is interrupted by a signal
      if (nanosleep(&ts,NULL) != -1)
      {
         elapsedTime += m_minPeriod;

         for (unsigned int i = 0 ; i < m_periodicProbes.size() ; i++)
         {
            unsigned int period = m_periodicProbes[i]->getPeriod();
            long int stepTime = elapsedTime % period;
            long int relativeTime = stepTime - period;
            if ( relativeTime < 5 && relativeTime > -5 )
            {
               m_periodicProbes[i]->update();
            }
         }   
      }
   }
}


const char* ProbeDataCollector::getLabel(unsigned int index)const
{
    assert(index < m_pProbes->size());

    return (*m_pProbes)[index]->getLabel();
}
