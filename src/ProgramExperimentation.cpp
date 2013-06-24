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

struct ThreadArgs
{
    ProgramExperimentation* pExp;
    ProgramRunner* pRunner;
    unsigned int threadNumber;

    ThreadArgs(ProgramExperimentation* pExp, ProgramRunner* pRunner, unsigned int threadNumber)
        :pExp(pExp),pRunner(pRunner),threadNumber(threadNumber) {}
};

void* runnerThread(void* pArgs)
{
    ThreadArgs* pTArgs = reinterpret_cast<ThreadArgs*>(pArgs);

    std::vector<unsigned int> pinning(pTArgs->pExp->m_options.getPinning());
    if ( pinning.size() != 0 )
    {
       // Pin it
       CPUUtils::pinCPU(pinning[pTArgs->threadNumber]);
    }
    CPUUtils::setFifoMaxPriority(-1);

    pTArgs->pRunner->start(pTArgs->pExp->m_pOverheadResults,
                           pTArgs->pExp->m_pResults,
                           pTArgs->threadNumber);

    return NULL;
}

ProgramExperimentation::ProgramExperimentation(const Options &options)
    :Experimentation(options)
{

}

void ProgramExperimentation::start()
{
    // Gets the options to run the test
    const std::vector<std::string>& args = m_options.getArgs();

    unsigned int nbRepet(m_options.getNbRepetition());
    unsigned int nbProcess(m_options.getNbProcess());


    ProgramRunner run(m_pProbeDataCollector, m_execFile, args, nbProcess, m_options.getNbMetaRepetition());
    std::vector<std::pair<pthread_t,ThreadArgs*> > threads;

    for ( unsigned int repet = 0 ; repet < nbRepet ; repet++ )
    {
       for ( unsigned int process = 0 ; process < nbProcess ; process++ )
       {
           pthread_t threadId;
           pthread_attr_t attr;
           ThreadArgs* pArgs = new ThreadArgs(this, &run,process);

           if (pthread_attr_init(&attr) != 0 )
           {
               std::cerr << "Failed to init attributes for thread" << std::endl;
               throw std::runtime_error("pthread_attr_init");
           }
           if ( pthread_create(&threadId,&attr,runnerThread,pArgs) != 0 )
           {
               std::cerr << "Failed to create runner thread" << std::endl;
               throw std::runtime_error("pthread_create");
           }

           threads.push_back(std::pair<pthread_t,ThreadArgs*>(threadId,pArgs));
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
    }

    saveResults();
}
