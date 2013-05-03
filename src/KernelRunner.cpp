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

KernelRunner::KernelRunner(const std::vector<std::string>& probePaths, const std::string& resultFileName, void* pKernelFct, unsigned long int nbKernelIteration, size_t memorySize, unsigned int nbProcess, unsigned int nbMetaRepet)
   :m_resultFile(resultFileName.c_str()),m_pKernelFct(reinterpret_cast<KernelFctPtr>(pKernelFct)),m_nbMetaRepet(nbMetaRepet),m_nbProcess(nbProcess),m_memorySize(memorySize),m_nbKernelIteration(nbKernelIteration)
{
   assert(m_pKernelFct);
   
   for ( std::vector<std::string>::const_iterator itPath = probePaths.begin() ; itPath != probePaths.end() ; ++itPath )
   {
      m_probes.push_back(new Probe(*itPath));
   }
   
   m_runResults.resize(m_nbProcess, std::vector< std::vector < std::pair<double, double> > >(m_nbMetaRepet, std::vector<std::pair<double, double> >(m_probes.size(),std::pair<double, double>(0,0))));
   
   m_memory.resize(m_nbProcess,0);
   for ( std::vector<char*>::iterator itMem = m_memory.begin() ; itMem != m_memory.end() ; ++itMem )
   {
      *itMem = new char[m_memorySize * m_nbKernelIteration];
      memset(*itMem,0,m_memorySize * m_nbKernelIteration);
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
}

KernelRunner::~KernelRunner()
{
   for ( ProbeList::const_iterator itProbe = m_probes.begin() ; itProbe != m_probes.end() ; ++itProbe )
   {
      delete *itProbe;
   }
      
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
}

void KernelRunner::flushCaches(unsigned int nbProcess)
{
   char c = 0;
   for (size_t i = 0 ; i < m_memorySize ; i++)
   {
      c += m_memory[nbProcess][i];
   }
   
   m_memory[nbProcess][0] = c/m_memorySize;
}

void KernelRunner::evaluation(GlobalResultsArray& resultArray, const std::vector<char*>& memory, unsigned long int nbKernelIteration, size_t size, unsigned int metaRepet, unsigned int processNumber)
{
   bool broken = false;
   int i = 0;
   
   do
   {
      broken = false;
      for (i = 0; i < m_probes.size() ; i++) /* Eval Start */
      {
         resultArray[processNumber][metaRepet][i].first = m_probes[i]->startMeasure();
      }
      
      startTest(memory, nbKernelIteration, size, processNumber);
      
      for (i = m_probes.size()-1 ; i >= 0; i--) /* Eval Stop */
      {
         resultArray[processNumber][metaRepet][i].second = m_probes[i]->stopMeasure();
         
         if ( resultArray[processNumber][metaRepet][i].second - resultArray[processNumber][metaRepet][i].first < 0 )
         {
            broken = true;
         }
      }

      // ensure all the tasks finished before going to the next iteration
      if (processNumber == 0)
      {
         for ( unsigned int i = 0 ; i < m_nbProcess - 1; i++ )
         {
            if ( sem_wait(m_fatherLock) != 0 )
            {
               perror("sem_wait father");
            }
         }

         for ( unsigned int i = 0 ; i < m_nbProcess - 1; i++ )
         {
            if ( sem_post(m_processEndLock) != 0 )
            {
               perror("sem_post processEnd");
            }
         }
      }
      else
      {
         if ( sem_post(m_fatherLock) != 0 )
         {
            perror("sem_post");
         }
      
         if ( sem_wait(m_processEndLock) != 0 )
         {
            perror("sem_wait");
         }
      }


   } while(broken);
}

void KernelRunner::startTest(const std::vector<char*>& memory, unsigned long int nbKernelIteration, size_t size, unsigned int processNumber)
{
   pid_t pid = -1;
   switch (pid = fork ())
	{
		case -1:
		{
			exit (EXIT_FAILURE);
		}
		case 0:
		{
         CPUUtils::setFifoMaxPriority(-2);
         if ( sem_post(m_fatherLock) != 0 )
         {
            perror("sem_post");
         }
         
         if ( sem_wait(m_processLock) != 0 )
         {
            perror("sem_wait");
         }

         m_pKernelFct(nbKernelIteration, memory[processNumber], size);
         exit(EXIT_SUCCESS);
      }
      default:
		{
			// Father
			int status;
         
         // synchronized start
         if ( processNumber == 0 )
         {
            for ( unsigned int i = 0 ; i < m_nbProcess ; i++ )
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
         }
         
			waitpid (pid, &status, 0);
			
			int res = WEXITSTATUS (status);
			/* Child must end normally */
			if (WIFEXITED (status) == 0)
			{
				if (WIFSIGNALED (status))
				{
					 psignal (WTERMSIG (status), "Error: Input benchmark performed an error, exiting now...");
					 exit (EXIT_FAILURE);
				}
				std::cerr << "Benchmark ended non-normally, exiting now..." << std::endl;
				exit (EXIT_FAILURE);
			}
         
			break;
		}
   }
}

void KernelRunner::saveResults()
{
   // We save only the results for the first process
   std::vector< std::vector<double> > runResults(m_runResults[0].size(),std::vector<double>(m_probes.size(),0));
   for ( unsigned int mRepet = 0 ; mRepet < m_runResults[0].size() ; mRepet++ )
   {
      for ( unsigned int i = 0 ; i < m_runResults[0][mRepet].size() ; i++ )
      {
         runResults[mRepet][i] += (m_runResults[0][mRepet][i].second - m_runResults[0][mRepet][i].first);
         
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

void KernelRunner::start(unsigned int processNumber)
{
   for (unsigned int metaRepet = 0; metaRepet < m_nbMetaRepet ; metaRepet++)
	{
      if ( processNumber == 0 )
      {
         std::cout << "Repetition - " << metaRepet << "\r";
      }
      
      flushCaches(processNumber);
      evaluation(m_runResults,m_memory,m_nbKernelIteration,m_memorySize,metaRepet,processNumber);
   }
   
   if ( processNumber == 0 )
   {
      saveResults();
      std::cout << std::endl;
   }
}
