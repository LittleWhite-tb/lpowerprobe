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

#ifndef RUNNER_HPP
#define RUNNER_HPP

#include <vector>
#include <string>
#include <fstream>

#include <semaphore.h>

#include "Probe.hpp"

/**
 * Handler to run and benchmark a test
 * The class load all the probes passed to the constructor. Before starting
 * the real test with \a start(), a overhead benchmarking is done to estimate the libraries
 * costs. Finally, the real test is launch N metarepetition and the results
 * are outputted in a file. The results gets the overhead removed before getting
 * outputted.
 */
class Runner
{
private:
   std::ofstream m_resultFile;   /*!< */

   ProbeList* m_pProbes;  /*!< Probe to use during a test */
   
   
   unsigned int m_nbMetaRepet;/*!< Number of repetition to do */
   unsigned int m_nbProcess;  /*!< Number of process started */
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
    * \param test the program to evaluate
    * \param args the args to pass to the program to evaluate
    * \param metaRepet the actual repetition number
    * \param processNumber the actual process number
    */
   void evaluation(GlobalResultsArray& resultArray, const std::string& test, const std::vector<std::string>& args, unsigned int metaRepet, unsigned int processNumber);
   
   /**
    * Start the real test in a fork. Synchonises all active process starting the test
    * \param programName the program to evaluate
    * \param pArgv the args to pass to the program to evaluate
    * \param processNumber the actual process number
    */
   void startTest(const std::string& programName, char** pArgv, unsigned int processNumber);
   
   /**
    * Saves the result in a file after applying the overhead bias
    */
   void saveResults();

public:
   /**
    * Prepare a benchmark run
    * Loads the probes, allocates results table memory
    * \param pProbes list of probes to use
    * \param resultFileName file where to output the results
    * \param nbProcess the number of process that will be started
    * \param nbMetaRepet the number of meta repetition to run
    */
   Runner(ProbeList* pProbes, const std::string& resultFileName, unsigned int nbProcess, unsigned int nbMetaRepet);
   
   /**
    */
   ~Runner();

   /**
    * Start the benchmark
    * \param test the program to bench
    * \param args the args to pass to the program to bench
    * \param processNumber the process number
    */
   void start(const std::string& test, const std::vector<std::string>& args, unsigned int processNumber);
};

#endif
