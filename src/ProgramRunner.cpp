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

#include "ProgramRunner.hpp"

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

ProgramRunner::ProgramRunner(ProbeList* pProbes, const std::string& resultFileName, const std::string& test, const std::vector<std::string>& args, unsigned int nbProcess, unsigned int nbMetaRepet)
    :Runner(pProbes,resultFileName,nbProcess,nbMetaRepet),
      m_test(test),m_args(args)
{

}

ProgramRunner::~ProgramRunner()
{

}

void ProgramRunner::calculateOverhead(unsigned int metaRepet, unsigned int processNumber)
{
   std::vector<std::string> emptyArgs;
   evaluation(m_overheadResults, INSTALL_DIR "/share/lPowerProbe/empty", emptyArgs,metaRepet,processNumber);
}

void ProgramRunner::evaluation(GlobalResultsArray& resultArray, const std::string& test, const std::vector<std::string>& args, unsigned int metaRepet, unsigned int processNumber)
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

void ProgramRunner::startTest(const std::string& programName, char** pArgv, unsigned int processNumber)
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

void ProgramRunner::start(unsigned int processNumber)
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
      evaluation(m_runResults,m_test,m_args,metaRepet,processNumber);
   }

   if ( processNumber == 0 )
   {
      saveResults();
   }
}
