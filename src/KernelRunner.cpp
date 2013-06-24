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

#include "StringUtils.hpp" 
#include "CPUUtils.hpp" 

KernelRunner::KernelRunner(ProbeDataCollector* pProbesDataCollector, void* pKernelFct, void* pDummyKernelFct, unsigned long int nbKernelIteration, size_t iterationMemorySize, unsigned int nbProcess, unsigned int nbMetaRepet)
    :Runner(pProbesDataCollector,nbProcess,nbMetaRepet),
     m_pKernelFct(reinterpret_cast<KernelFctPtr>(pKernelFct)),m_pDummyKernelFct(reinterpret_cast<KernelFctPtr>(pDummyKernelFct)),m_iterationMemorySize(iterationMemorySize),m_nbKernelIteration(nbKernelIteration)
{
   assert(m_pKernelFct);

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
}

KernelRunner::~KernelRunner()
{
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

void KernelRunner::calculateOverhead(ExperimentationResults* pOverheadExpResult, unsigned int metaRepet, unsigned int processNumber)
{
   evaluation(pOverheadExpResult,m_pDummyKernelFct,m_overheadMemory,OVERHEAD_KERNELITER,m_iterationMemorySize,metaRepet,processNumber);
}

void KernelRunner::evaluation(ExperimentationResults* pExpResult, KernelFctPtr pKernelFct, const std::vector<char*>& memory, unsigned long int nbKernelIteration, size_t size, unsigned int metaRepet, unsigned int processNumber)
{
    if ( sem_post(m_fatherLock) != 0 )
    {
        perror("sem_post");
    }

    if ( sem_wait(m_processLock) != 0 )
    {
        perror("sem_wait");
    }

    if ( processNumber == 0 )
    {
        m_pProbesDataCollector->start();
    }

    pKernelFct(nbKernelIteration, memory[processNumber], size);

    if ( processNumber == 0 )
    {
        m_pProbesDataCollector->stop(pExpResult);
    }

    if ( sem_post(m_fatherLock) != 0 )
    {
        perror("sem_post");
    }

    if ( sem_wait(m_processEndLock) != 0 )
    {
        perror("sem_wait");
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

void KernelRunner::start(ExperimentationResults* pOverheadExpResult, ExperimentationResults* pExpResult, unsigned int processNumber)
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
            calculateOverhead(pOverheadExpResult,metaRepet,processNumber);
         }

         for (unsigned int metaRepet = 0; metaRepet < m_nbMetaRepet ; metaRepet++)
         {
            if ( processNumber == 0 )
            {
               std::cout << "Repetition - " << metaRepet << "\r";
            }
            
            flushCaches(processNumber);
            evaluation(pExpResult,m_pKernelFct,m_memory,m_nbKernelIteration,m_iterationMemorySize,metaRepet,processNumber);
         }
      }
   }
   
   if ( processNumber == 0 )
   {
      kill(pid,SIGTERM);
   }
}
