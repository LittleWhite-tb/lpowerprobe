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
#include <string>
#include <fstream>

#include <semaphore.h>

#include "Probe.hpp"

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
class KernelRunner
{
private:
   static const unsigned long int OVERHEAD_KERNELITER = 1;

   typedef unsigned long (*KernelFctPtr)(size_t, void*, unsigned);
   std::ofstream m_resultFile;   /*!< */

   ProbeList* m_pProbes; /*!< Probes to use to benchmark */
   KernelFctPtr m_pKernelFct; /*!< Kernel to bench */
   KernelFctPtr m_pDummyKernelFct; /*!< Special kernel to calculate the overhead */
   
   unsigned int m_nbMetaRepet;/*!< Number of repetition to do */
   unsigned int m_nbProcess;  /*!< Number of process started */
   
   // Contain the memory segment for each process
   std::vector<char*> m_overheadMemory; /*!< Memory chunk allocated for dummy kernel */
   std::vector<char*> m_memory; /*!< Memory chunk where to work for kernel */
   
   size_t m_iterationMemorySize; /*!< Size of the memory used by one kernel iteration */
   size_t m_overheadMemorySize;  /*!< Size of the memory used by one kernel iteration (for dummy kernel) */
   
   size_t m_nbKernelIteration;   /*!< Number of time the inner kernel loop will run */
   
   // Contains results for 
   // - All process
   // - All meta repease
   // - All probes
   typedef std::vector < std::vector<std::vector<std::pair<double, double> > > > GlobalResultsArray;
   GlobalResultsArray m_overheadResults;  /*!< Probe results for the overhead test */
   GlobalResultsArray m_runResults; /*!< Probe results for the run test */
   
   sem_t* m_fatherLock;       /*!< Process test synchronisation */
   sem_t* m_processLock;      /*!< Process test synchronisation */
   sem_t* m_processEndLock;   /*!< Process end synchronisation */
   
   /**
    * Starter for the overhead benchmark
    * Calls evaluation with a specific test 'empty'
    * \param metaRepet actual repetition number
    * \param processNumber actual process id running this function
    */
   void calculateOverhead(unsigned int metaRepet, unsigned int processNumber);
   
   /**
    * Benchmark a test by start the measurements from the probes, running the test, and saving the probes results
    * \param resultArray the memory array to keep the results
    * \param pKernelFct the kernel to evaluate
    * \param memory the space where to apply the bench
    * \param nbKernelIteration the number of inner kernel loop iteration
    * \param size the size used by one kernel iteration
    * \param metaRepet the actual repetition number
    * \param processNumber the actual process number
    */
   void evaluation(GlobalResultsArray& resultArray, KernelFctPtr pKernelFct, const std::vector<char*>& memory, unsigned long int nbKernelIteration, size_t size, unsigned int metaRepet, unsigned int processNumber);
   
   /**
    * Saves the result in a file after applying the overhead bias
    */
   void saveResults();
   
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
    * Loads the probes, allocates results table memory
    * \param pProbes list of probes to use
    * \param resultFileName file where to output the results
    * \param pKernelFct the kernel to bench
    * \param pDummyKernelFct the dummy kernel to calculate probes overload
    * \param nbKernelIteration the number of iteration spent in the kernel
    * \param iterationMemorySize the size of the memory segment where to apply the kernel
    * \param nbProcess the number of process that will be started
    * \param nbMetaRepet the number of meta repetition to run
    */
   KernelRunner(ProbeList* pProbes, const std::string& resultFileName, void* pKernelFct, void* pDummyKernelFct, unsigned long int nbKernelIteration, size_t iterationMemorySize, unsigned int nbProcess, unsigned int nbMetaRepet);
   
   /**
    */
   ~KernelRunner();

   /**
    * Start the benchmark
    * \param processNumber the process number
    */
   void start(unsigned int processNumber);
};

#endif
