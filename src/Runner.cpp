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

#include "Runner.hpp"

#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h> 

#include "StringUtils.hpp" 

#include "CPUUtils.hpp" 

#include "RunData.hpp"
#include "ProbeData.hpp"

Runner::Runner(ProbeDataCollector* pProbesDataCollector, const std::string& resultFileName, unsigned int nbProcess, unsigned int nbMetaRepet)
   :m_resultFile(resultFileName.c_str()),m_pProbesDataCollector(pProbesDataCollector),m_nbMetaRepet(nbMetaRepet),m_nbProcess(nbProcess)
{
   m_overheadResults.resize(m_nbProcess, std::vector< std::vector < RunData* > >(m_nbMetaRepet, std::vector<RunData*>(m_pProbesDataCollector->getNumberProbes(),NULL)));
   m_runResults.resize(m_nbProcess, std::vector< std::vector < RunData* > >(m_nbMetaRepet, std::vector<RunData*>(m_pProbesDataCollector->getNumberProbes(),NULL)));
   
   // Get pid to put in semaphores names
   m_pid = getpid();
   // Convert it
   if ( StringUtils::to_string(m_pid,m_pidString) == false )
   {
      std::cerr << "Fail to transform PID in a string" << std::endl;
      m_pidString = "00000";
   }
   
   
   // open fatherLock
   int shareSeg;
   if ((shareSeg = shm_open(("/shmFather" + m_pidString).c_str(), O_RDWR | O_CREAT, S_IRWXU)) < 0) 
   {
      perror("shm_open");
      exit(1);
   }

  if ( ftruncate(shareSeg, sizeof(sem_t)) < 0 ) {
    perror("ftruncate");
    exit(1);
  }

  if ((m_fatherLock = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,MAP_SHARED, shareSeg, 0)) == MAP_FAILED) 
  {
      perror("mmap");
      exit(1);
   }
   
   if ( sem_init(m_fatherLock, 1, 0) != 0 )
   {
      std::cerr << "Fail to create semaphore, process will not be synced" << std::endl;
      perror("sem_init");
   }
   
   // open processLock
   if ((shareSeg = shm_open(("/shmProcess" + m_pidString).c_str(), O_RDWR | O_CREAT, S_IRWXU)) < 0) 
   {
      perror("shm_open");
      exit(1);
   }

  if ( ftruncate(shareSeg, sizeof(sem_t)) < 0 ) {
    perror("ftruncate");
    exit(1);
  }

  if ((m_processLock = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,MAP_SHARED, shareSeg, 0)) == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
   
   if ( sem_init(m_processLock, 1, 0) != 0 )
   {
      std::cerr << "Fail to create semaphore, process will not be synced" << std::endl;
      perror("sem_init");
   }

   // open processEndLock
   if ((shareSeg = shm_open(("/shmProcessEnd" + m_pidString).c_str(), O_RDWR | O_CREAT, S_IRWXU)) < 0) 
   {
      perror("shm_open");
      exit(1);
   }

  if ( ftruncate(shareSeg, sizeof(sem_t)) < 0 ) {
    perror("ftruncate");
    exit(1);
  }

  if ((m_processEndLock = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shareSeg, 0)) == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
   
   if ( sem_init(m_processEndLock, 1, 0) != 0 )
   {
      std::cerr << "Fail to create semaphore, process will not be synced" << std::endl;
      perror("sem_init");
   }
}

Runner::~Runner()
{
   sem_destroy(m_processEndLock);
   shm_unlink(("/shmProcessEnd" + m_pidString).c_str());
   
   sem_destroy(m_processLock);
   shm_unlink(("/shmProcess" + m_pidString).c_str());
   
   sem_destroy(m_fatherLock);
   shm_unlink(("/shmFather" + m_pidString).c_str());
}

void Runner::saveResults()
{
   // We save only the results for the first process
   std::vector<ProbeData*> libsOverheadAvg(m_pProbesDataCollector->getNumberProbes(),NULL);
   for ( std::vector<std::vector<RunData*> >::const_iterator itMRepet = m_overheadResults[0].begin() ;
         itMRepet != m_overheadResults[0].end() ; ++itMRepet )
   {
      for ( unsigned int i = 0 ; i < itMRepet->size() ; i++ )
      {
          const ProbeData& pd = (*itMRepet)[i]->getProbeData(0);
          if ( libsOverheadAvg[i] == NULL )
          {
              libsOverheadAvg[i] = new ProbeData(pd.getNbDevices(),pd.getNbChannels());
          }

          // We only manage the first (single shot) libraries
          (*libsOverheadAvg[i]) += pd;
      }
   }

   for ( std::vector<ProbeData*>::iterator itData = libsOverheadAvg.begin() ; itData != libsOverheadAvg.end() ; ++itData )
   {
      (*(*itData)) /= m_overheadResults[0].size();
   }

   for ( unsigned int mRepet = 0 ; mRepet < m_runResults[0].size() ; mRepet++ )
   {
      for ( unsigned int i = 0 ; i < m_runResults[0][mRepet].size() ; i++ )
      {
          const ProbeData& rawProbeData = m_runResults[0][mRepet][i]->getProbeData(0);

          ProbeData* pPD = rawProbeData -(*libsOverheadAvg[i]);

        m_resultFile << *pPD;
        if ( i !=  m_runResults[0][mRepet].size()-1 )
        {
            m_resultFile << ";";
        }
        else
        {
          m_resultFile << std::endl;
        }

        delete pPD;
      }
   }

   for ( std::vector<ProbeData*>::iterator itData = libsOverheadAvg.begin() ; itData != libsOverheadAvg.end() ; ++itData )
   {
       delete *itData;
   }
}
