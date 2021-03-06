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

#include "KernelExperimentation.hpp"

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

#include "KernelCompiler.hpp"
#include "KernelRunner.hpp"
#include "Kernel.hpp"
#include "CPUUtils.hpp"
#include "ExperimentationThreadArgs.hpp"

const std::string KernelExperimentation::DUMMY_KERNEL_FILE = DATA_DIR "/empty.s";

void* kernelRunnerThread(void* pArgs)
{
    ExperimentationThreadArgs* pTArgs = reinterpret_cast<ExperimentationThreadArgs*>(pArgs);

    KernelExperimentation* pKExp = dynamic_cast<KernelExperimentation*>(pTArgs->pExp);
    assert(pKExp);
    pKExp->runStarter(pTArgs);

    return NULL;
}

void KernelExperimentation::runStarter(ExperimentationThreadArgs* pTArgs)
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

KernelExperimentation::KernelExperimentation(const Options &options)
    :Experimentation(options)
{
   std::string kernelFile;
   if ( !KernelCompiler::compile(options.getExecName(),kernelFile) )
   {
      throw KernelCompilationException("Kernel compilation failed : abort");
   }

   if ( !KernelCompiler::compile(DUMMY_KERNEL_FILE,m_dummyKernelFile) )
   {
      throw KernelCompilationException("Dummy Kernel compilation failed : abort");
   }

   m_execFile = kernelFile;
}

void KernelExperimentation::start()
{
    Kernel kernel(m_execFile);
    void* pKernelFct = kernel.loadFunction("entryPoint");
    if ( pKernelFct == NULL )
    {
       // I believe that loadKernelFCT will print error message
       return;
    }

    Kernel dummyKernel(m_dummyKernelFile);
    void* pDummyKernelFct = kernel.loadFunction("entryPoint");
    if ( pDummyKernelFct == NULL )
    {
       // I believe that loadKernelFCT will print error message
       return;
    }

    // Gets the options to run the test
    unsigned int nbProcess(m_options.getNbProcess());

    KernelRunner run(m_pProbeDataCollector, pKernelFct, pDummyKernelFct, m_options.getNbKernelIteration(), m_options.getIterationMemorySize(), nbProcess, m_options.getNbRepetition());
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
        if ( pthread_create(&threadId,&attr,kernelRunnerThread,pArgs) != 0 )
        {
            std::cerr << "Failed to create runner thread" << std::endl;
            throw std::runtime_error("pthread_create");
        }

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
