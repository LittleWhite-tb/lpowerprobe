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

#ifndef KERNELRUNNER_HPP
#define KERNELRUNNER_HPP

#include <vector>

#include "Probe.hpp"

#include "Runner.hpp"

/**
 * \todo Actually, this file is a massive copy paste from Runner
 * 
 * Handler to run and benchmark a test
 * The class load all the probes passed to the constructor. Before starting
 * the real test with \a start(), a overhead benchmarking is done to estimate the libraries
 * costs. Finally, the real test is launch N metarepetition and the results
 * are outputted in a file. The results gets the overhead removed before getting
 * outputted.
 */
class KernelRunner : public Runner
{
private:
   static const unsigned long int OVERHEAD_KERNELITER = 1;

   typedef unsigned long (*KernelFctPtr)(size_t, void*, unsigned);

   KernelFctPtr m_pKernelFct; /*!< Kernel to bench */
   KernelFctPtr m_pDummyKernelFct; /*!< Special kernel to calculate the overhead */
   
   // Contain the memory segment for each process
   std::vector<char*> m_overheadMemory; /*!< Memory chunk allocated for dummy kernel */
   std::vector<char*> m_memory; /*!< Memory chunk where to work for kernel */
   
   size_t m_iterationMemorySize; /*!< Size of the memory used by one kernel iteration */
   size_t m_overheadMemorySize;  /*!< Size of the memory used by one kernel iteration (for dummy kernel) */
   
   size_t m_nbKernelIteration;   /*!< Number of time the inner kernel loop will run */
   

   
   /**
    * Starter for the overhead benchmark
    * Calls evaluation with a specific test 'empty'
    * \param pOverheadExpResult pointer on \a ExperimentationResults where to keep the probes results
    * \param processNumber actual process id running this function
    */
   void calculateOverhead(ExperimentationResults* pOverheadExpResult, unsigned int processNumber);
   
   /**
    * Benchmark a test by start the measurements from the probes, running the test, and saving the probes results
    * \param pExpResult pointer on \a ExperimentationResults where to keep the probes results
    * \param pKernelFct the kernel to evaluate
    * \param memory the space where to apply the bench
    * \param nbKernelIteration the number of inner kernel loop iteration
    * \param size the size used by one kernel iteration
    * \param processNumber the actual process number
    */
   void evaluation(ExperimentationResults* pExpResult, KernelFctPtr pKernelFct, const std::vector<char*>& memory, unsigned long int nbKernelIteration, size_t size, unsigned int processNumber);
   
   /**
    * Infinite loop called to sync the bench processes
    */
   void syncLoop();
   
   /**
    * play with the array (aka : flushing caches)
    * \param nbProcess the number of process that will be started
    */
   void flushCaches(unsigned int nbProcess);

public:
   /**
    * Prepare a benchmark run
    * \param pProbesDataCollector a pointer to the \a ProbeDataCollector to use
    * \param pKernelFct the kernel to bench
    * \param pDummyKernelFct the dummy kernel to calculate probes overload
    * \param nbKernelIteration the number of iteration spent in the kernel
    * \param iterationMemorySize the size of the memory segment where to apply the kernel
    * \param nbProcess the number of process that will be started
    * \param nbMetaRepet the number of meta repetition to run
    */
   KernelRunner(ProbeDataCollector* pProbesDataCollector, void* pKernelFct, void* pDummyKernelFct, unsigned long int nbKernelIteration, size_t iterationMemorySize, unsigned int nbProcess, unsigned int nbMetaRepet);
   
   /**
    */
   ~KernelRunner();

   void start(ExperimentationResults* pOverheadExpResult, ExperimentationResults* pExpResult, unsigned int processNumber);
};

#endif
