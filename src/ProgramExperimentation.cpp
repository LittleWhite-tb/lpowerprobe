/*
 * lPowerProbe - A light benchmark tool oriented for energy probing
 *               heavely based on likwid
 * Copyright (C) 2013-2014 Universite de Versailles
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

#include "ProgramExperimentation.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <cstdlib>
#include <cstdio>
#include <exception>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>


#include "Kernel.hpp"
#include "ProgramRunner.hpp"
#include "CPUUtils.hpp"
#include "ExperimentationThreadArgs.hpp"

void* programRunnerThread(void* pArgs)
{
    ExperimentationThreadArgs* pTArgs = reinterpret_cast<ExperimentationThreadArgs*>(pArgs);

    ProgramExperimentation* pPExp = dynamic_cast<ProgramExperimentation*>(pTArgs->pExp);
    assert(pPExp);
    pPExp->runStarter(pTArgs);

    return NULL;
}

void ProgramExperimentation::runStarter(ExperimentationThreadArgs* pTArgs)
{
    std::vector<unsigned int> pinning(m_options.getPinning());
    if ( pinning.size() != 0 )
    {
       // Pin it
       CPUUtils::pinCPU(pinning[pTArgs->threadNumber]);
    }
    CPUUtils::setFifoMaxPriority(-1);

    pTArgs->pRunner->start(m_pOverheadResults,
                           m_pResults,
                           pTArgs->threadNumber);
}

ProgramExperimentation::ProgramExperimentation(const Options &options)
    :Experimentation(options)
{

}

void ProgramExperimentation::start()
{
   // Gets the options to run the test
   const std::vector<std::string>& args = m_options.getArgs();

   unsigned int nbProcess(m_options.getNbProcess());

   ProgramRunner run(m_pProbeDataCollector, m_execFile, args,m_options.getProgramEnv(), nbProcess, m_options.getNbRepetition());
   std::vector<std::pair<pthread_t,ExperimentationThreadArgs*> > threads;

   for ( unsigned int process = 0 ; process < nbProcess ; process++ )
   {
      pthread_t threadId;
      pthread_attr_t attr;
      ExperimentationThreadArgs* pArgs = new ExperimentationThreadArgs(this, &run,process);

      if (pthread_attr_init(&attr) != 0 )
      {
         std::cerr << "Failed to init attributes for thread" << std::endl;
         throw std::runtime_error("pthread_attr_init");
      }
      if ( pthread_create(&threadId,&attr,programRunnerThread,pArgs) != 0 )
      {
         std::cerr << "Failed to create runner thread" << std::endl;
         throw std::runtime_error("pthread_create");
      }

      pthread_attr_destroy(&attr);

      threads.push_back(std::pair<pthread_t,ExperimentationThreadArgs*>(threadId,pArgs));
   }

   // Wait for all the child to finish
   for (unsigned int i = 0 ; i < nbProcess ; i++)
   {
      void* pReturn;
      int status = pthread_join(threads[i].first,&pReturn);
      if ( status != 0 )
      {
         std::cerr << "Child exited with status " << status << ", an error occured.\n" << std::endl;
         perror("pthread_join");
      }

      delete threads[i].second;
   }

   saveResults();
}
