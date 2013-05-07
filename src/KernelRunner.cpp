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

#include "KernelRunner.hpp"

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

#include "CPUUtils.hpp" 

KernelRunner::KernelRunner(ProbeList* pProbes, const std::string& resultFileName, void* pKernelFct, void* pDummyKernelFct, unsigned long int nbKernelIteration, size_t iterationMemorySize, unsigned int nbProcess, unsigned int nbMetaRepet)
   :m_resultFile(resultFileName.c_str()),m_pProbes(pProbes),m_pKernelFct(reinterpret_cast<KernelFctPtr>(pKernelFct)),m_pDummyKernelFct(reinterpret_cast<KernelFctPtr>(pDummyKernelFct)),m_nbMetaRepet(nbMetaRepet),m_nbProcess(nbProcess),m_iterationMemorySize(iterationMemorySize),m_overheadMemorySize(iterationMemorySize),m_nbKernelIteration(nbKernelIteration)
{
   assert(m_pKernelFct);
   
   m_overheadResults.resize(m_nbProcess, std::vector< std::vector < std::pair<double, double> > >(m_nbMetaRepet, std::vector<std::pair<double, double> >(m_pProbes->size(),std::pair<double, double>(0,0))));
   m_runResults.resize(m_nbProcess, std::vector< std::vector < std::pair<double, double> > >(m_nbMetaRepet, std::vector<std::pair<double, double> >(m_pProbes->size(),std::pair<double, double>(0,0))));
   
   m_memory.resize(m_nbProcess,0);
   for ( std::vector<char*>::iterator itMem = m_memory.begin() ; itMem != m_memory.end() ; ++itMem )
   {
      *itMem = new char[m_iterationMemorySize * m_nbKernelIteration];
      memset(*itMem,0,m_iterationMemorySize * m_nbKernelIteration);
   }
   
   m_overheadMemory.resize(m_nbProcess,0);
   for ( std::vector<char*>::iterator itMem = m_overheadMemory.begin() ; itMem != m_overheadMemory.end() ; ++itMem )
   {
      *itMem = new char[m_overheadMemorySize * OVERHEAD_KERNELITER];
      memset(*itMem,0,m_overheadMemorySize * OVERHEAD_KERNELITER);
   }
   
   // open fatherLock
   int shareSeg;
   if ((shareSeg = shm_open("/shmFather", O_RDWR | O_CREAT, S_IRWXU)) < 0) 
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
   if ((shareSeg = shm_open("/shmProcess", O_RDWR | O_CREAT, S_IRWXU)) < 0) 
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
   if ((shareSeg = shm_open("/shmProcessEnd", O_RDWR | O_CREAT, S_IRWXU)) < 0) 
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
   
   
   // Special formatting for output file
   m_resultFile << std::fixed;
}

KernelRunner::~KernelRunner()
{
   sem_destroy(m_processEndLock);
   shm_unlink("/shmProcessEnd");
   
   sem_destroy(m_processLock);
   shm_unlink("/shmProcess");
   
   sem_destroy(m_fatherLock);
   shm_unlink("/shmFather");
   
   for ( std::vector<char*>::iterator itMem = m_memory.begin() ; itMem != m_memory.end() ; ++itMem )
   {
      delete [] *itMem;
   }
   for ( std::vector<char*>::iterator itMem = m_overheadMemory.begin() ; itMem != m_overheadMemory.end() ; ++itMem )
   {
      delete [] *itMem;
   }
}

void KernelRunner::flushCaches(unsigned int nbProcess)
{
   size_t size = m_iterationMemorySize*m_nbKernelIteration;
   char c = 0;
   
   if ( size == 0 )
   {
      return;
   }
   
   for (size_t i = 0 ; i < size ; i+=4096)
   {
      c += m_memory[nbProcess][i];
   }
   
   m_memory[nbProcess][0] = c/size;
}

void KernelRunner::calculateOverhead(unsigned int metaRepet, unsigned int processNumber)
{
   evaluation(m_overheadResults,m_pDummyKernelFct,m_overheadMemory,OVERHEAD_KERNELITER,m_iterationMemorySize,metaRepet,processNumber);
}

void KernelRunner::evaluation(GlobalResultsArray& resultArray, KernelFctPtr pKernelFct, const std::vector<char*>& memory, unsigned long int nbKernelIteration, size_t size, unsigned int metaRepet, unsigned int processNumber)
{
   bool broken = false;
   
   do
   {
      broken = false;
      
      if ( sem_post(m_fatherLock) != 0 )
      {
         perror("sem_post");
      }
      
      if ( sem_wait(m_processLock) != 0 )
      {
         perror("sem_wait");
      }
      
      for (size_t i = 0; i < m_pProbes->size() ; i++) /* Eval Start */
      {
         resultArray[processNumber][metaRepet][i].first = (*m_pProbes)[i]->startMeasure();
      }
      
      pKernelFct(nbKernelIteration, memory[processNumber], size);
   
      for (int i = m_pProbes->size()-1 ; i >= 0; i--) /* Eval Stop */
      {
         resultArray[processNumber][metaRepet][i].second = (*m_pProbes)[i]->stopMeasure();
         
         if ( resultArray[processNumber][metaRepet][i].second - resultArray[processNumber][metaRepet][i].first < 0 )
         {
            broken = true;
         }
      }
      
      
      if ( sem_post(m_fatherLock) != 0 )
      {
         perror("sem_post");
      }
      
      if ( sem_wait(m_processEndLock) != 0 )
      {
         perror("sem_wait");
      }
   } while(broken);
   
}

void KernelRunner::saveResults()
{
   // We save only the results for the first process
   std::vector<double> libsOverheadAvg(m_pProbes->size());
   for ( std::vector<std::vector<std::pair<double, double> > >::const_iterator itMRepet = m_overheadResults[0].begin() ;
         itMRepet != m_overheadResults[0].end() ; ++itMRepet )
   {
      for ( unsigned int i = 0 ; i < itMRepet->size() ; i++ )
      {
         libsOverheadAvg[i] += (*itMRepet)[i].second - (*itMRepet)[i].first;
      }
      
   }
   
   for ( std::vector<double>::iterator itLib = libsOverheadAvg.begin() ; itLib != libsOverheadAvg.end() ; ++itLib )
   {
      *itLib /= m_overheadResults[0].size();
   }
   
   std::vector< std::vector<double> > runResults(m_runResults[0].size(),std::vector<double>(m_pProbes->size(),0));
   for ( unsigned int mRepet = 0 ; mRepet < m_runResults[0].size() ; mRepet++ )
   {
      for ( unsigned int i = 0 ; i < m_runResults[0][mRepet].size() ; i++ )
      {
         runResults[mRepet][i] += (m_runResults[0][mRepet][i].second - m_runResults[0][mRepet][i].first) - libsOverheadAvg[i];
         
         m_resultFile << runResults[mRepet][i];
         if ( i !=  m_runResults[0][mRepet].size()-1 )
         {
            m_resultFile << ";";
         }
         else
         {
            m_resultFile << std::endl;
         }
      }
   }
}

void KernelRunner::syncLoop()
{
   while (true) // Will be killed by SIGTERM
   {
      for ( unsigned int i = 0 ; i < m_nbProcess  ; i++ )
      {
         if ( sem_wait(m_fatherLock) != 0 )
         {
            perror("sem_wait father");
         }
      }
      
      for ( unsigned int i = 0 ; i < m_nbProcess  ; i++ )
      {         
         if ( sem_post(m_processLock) != 0 )
         {
            perror("sem_post process");
         }
      }
      
      for ( unsigned int i = 0 ; i < m_nbProcess  ; i++ )
      {         
         if ( sem_wait(m_fatherLock) != 0 )
         {
            perror("sem_post process");
         }
      }
      
      for ( unsigned int i = 0 ; i < m_nbProcess  ; i++ )
      {         
         if ( sem_post(m_processEndLock) != 0 )
         {
            perror("sem_post process");
         }
      }
   }
}

void KernelRunner::start(unsigned int processNumber)
{
   pid_t pid = -1;
   if ( processNumber == 0)
   {
      pid = fork ();
   }
   else
   {
      pid = 1; // Parent, so, we will go in bench
   }
   
   switch (pid)
	{
      case -1:
      {
         std::cerr << "Synchronization fork failed" << std::endl;
         exit(EXIT_FAILURE);
      }
      case 0: // Child
      {
         syncLoop();
      }
      default: // Parent
      {
         for (unsigned int metaRepet = 0; metaRepet < m_nbMetaRepet ; metaRepet++)
         {
            flushCaches(processNumber);
            calculateOverhead(metaRepet,processNumber);
         }

         for (unsigned int metaRepet = 0; metaRepet < m_nbMetaRepet ; metaRepet++)
         {
            if ( processNumber == 0 )
            {
               std::cout << "Repetition - " << metaRepet << "\r";
            }
            
            flushCaches(processNumber);
            evaluation(m_runResults,m_pKernelFct,m_memory,m_nbKernelIteration,m_iterationMemorySize,metaRepet,processNumber);
         }
         
         if ( processNumber == 0 )
         {
            saveResults();
            std::cout << std::endl;
         }
      }
   }
   
   if ( processNumber == 0 )
   {
      kill(pid,SIGTERM);
   }
}
