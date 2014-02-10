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

#ifndef PROGRAMRUNNER_HPP
#define PROGRAMRUNNER_HPP

#include <string>


#include "Runner.hpp"

/**
 * Handler to run and benchmark a test
 * The class load all the probes passed to the constructor. Before starting
 * the real test with \a start(), a overhead benchmarking is done to estimate the libraries
 * costs. Finally, the real test is launch N metarepetition and the results
 * are outputted in a file. The results gets the overhead removed before getting
 * outputted.
 */
class ProgramRunner : public Runner
{
private:
    const std::string& m_test;
    const std::vector<std::string>& m_args;

   /**
    * Starter for the overhead benchmark
    * Calls evaluation with a specific test 'empty'
    * \param pOverheadExpResult pointer on \a ExperimentationResults where to keep the probes results
    * \param metaRepet actual repetition number
    * \param processNumber actual process id running this function
    */
   void calculateOverhead(ExperimentationResults* pOverheadResults, unsigned int processNumber);

   /**
    * Benchmark a test by start the measurements from the probes, running the test, and saving the probes results
    * \param pResults pointer on \a ExperimentationResults where to keep the probes results
    * \param test the program to evaluate
    * \param args the args to pass to the program to evaluate
    * \param metaRepet the actual repetition number
    * \param processNumber the actual process number
    */
   void evaluation(ExperimentationResults* pResults, const std::string& test, const std::vector<std::string>& args, unsigned int processNumber);

   /**
    * Start the real test in a fork. Synchonises all active process starting the test
    * \param programName the program to evaluate
    * \param pArgv the args to pass to the program to evaluate
    * \param processNumber the actual process number
    */
   void startTest(const std::string& programName, char** pArgv, unsigned int processNumber);

public:
   /**
    * Prepare a benchmark run
    * Loads the probes, allocates results table memory
    * \param pProbesDataCollector pointer to the \a ProbeDataCollector to use to collect the probes data
    * \param test the program to bench
    * \param args the args to pass to the program to bench
    * \param nbProcess the number of process that will be started
    * \param nbMetaRepet the number of meta repetition to run
    */
   ProgramRunner(ProbeDataCollector* pProbesDataCollector, const std::string& test, const std::vector<std::string>& args, unsigned int nbProcess, unsigned int nbMetaRepet);

   /**
    */
   ~ProgramRunner();

   void start(ExperimentationResults* pOverheadExpResult, ExperimentationResults* pExpResult, unsigned int processNumber);
};

#endif
