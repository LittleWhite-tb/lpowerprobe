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

#include "CPUUtils.hpp" 

Runner::Runner(ProbeList* pProbes, const std::string& resultFileName, unsigned int nbProcess, unsigned int nbMetaRepet)
   :m_resultFile(resultFileName.c_str()),m_pProbes(pProbes),m_nbMetaRepet(nbMetaRepet),m_nbProcess(nbProcess)
{
   m_overheadResults.resize(m_nbProcess, std::vector< std::vector < std::pair<double, double> > >(m_nbMetaRepet, std::vector<std::pair<double, double> >(m_pProbes->size(),std::pair<double, double>(0,0))));
   m_runResults.resize(m_nbProcess, std::vector< std::vector < std::pair<double, double> > >(m_nbMetaRepet, std::vector<std::pair<double, double> >(m_pProbes->size(),std::pair<double, double>(0,0))));
   
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

Runner::~Runner()
{
   sem_destroy(m_processEndLock);
   shm_unlink("/shmProcessEnd");
   
   sem_destroy(m_processLock);
   shm_unlink("/shmProcess");
   
   sem_destroy(m_fatherLock);
   shm_unlink("/shmFather");
}

void Runner::calculateOverhead(unsigned int metaRepet, unsigned int processNumber)
{
   std::vector<std::string> emptyArgs;
   evaluation(m_overheadResults,"./empty/empty", emptyArgs,metaRepet,processNumber);
}

void Runner::evaluation(GlobalResultsArray& resultArray, const std::string& test, const std::vector<std::string>& args, unsigned int metaRepet, unsigned int processNumber)
{
   bool broken = false;
   
   int nbArgs=0;
   char** pArgv = NULL;
         
   /* If argv == NULL, we have to create an argv table anyway... */
   if (args.size() == 0)
   {
      nbArgs = 1;
      pArgv = new char*[nbArgs+1];
      pArgv[0] = strdup(test.c_str());
      pArgv[1] = NULL;
   }
   /* Else, simply execute it with the argv table we got... */
   else
   {
      nbArgs = args.size();
      pArgv = new char*[nbArgs+2];
      pArgv[0] = strdup(test.c_str());
      
      size_t i = 1;
      for ( i = 1 ; i-1 < args.size()  ; i++ )
      {
         pArgv[i] = strdup(args[i-1].c_str());
      }
      pArgv[i] = NULL;
   }
   
   do
   {
      broken = false;
      for (size_t i = 0; i < m_pProbes->size() ; i++) /* Eval Start */
      {
         resultArray[processNumber][metaRepet][i].first = (*m_pProbes)[i]->startMeasure();
      }
      
      startTest(test, pArgv, processNumber);
      
      for (int i = m_pProbes->size()-1 ; i >= 0; i--) /* Eval Stop */
      {
         resultArray[processNumber][metaRepet][i].second = (*m_pProbes)[i]->stopMeasure();
         
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
   
   for (int i = 0 ; i < nbArgs+1 ; i++ )
   {
      free(pArgv[i]);
   }
   delete [] pArgv;
}

void Runner::startTest(const std::string& programName, char** pArgv, unsigned int processNumber)
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

         execvp (programName.c_str(), pArgv);
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
         if(res != EXIT_SUCCESS)
         {
            std::cerr << "Child exited with status " << res << ", an error occured.\n" << std::endl;
         }
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

void Runner::saveResults()
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

void Runner::start(const std::string& test, const std::vector<std::string>& args, unsigned int processNumber)
{
   for (unsigned int metaRepet = 0; metaRepet < m_nbMetaRepet ; metaRepet++)
	{
      calculateOverhead(metaRepet,processNumber);
   }
   
   for (unsigned int metaRepet = 0; metaRepet < m_nbMetaRepet ; metaRepet++)
	{
      if ( processNumber == 0 )
      {
         std::cout << "Repetition - " << metaRepet << std::endl;
      }
      evaluation(m_runResults,test,args,metaRepet,processNumber);
   }
   
   if ( processNumber == 0 )
   {
      saveResults();
   }
}
