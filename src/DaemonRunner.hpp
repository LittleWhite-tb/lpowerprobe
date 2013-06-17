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

#ifndef DAEMONRUNNER_HPP
#define DAEMONRUNNER_HPP

#include "Runner.hpp"

/**
 * Handler to run and benchmark a test
 * The class load all the probes passed to the constructor. Before starting
 * the real test with \a start(), a overhead benchmarking is done to estimate the libraries
 * costs. Finally, the real test is launch N metarepetition and the results
 * are outputted in a file. The results gets the overhead removed before getting
 * outputted.
 */
class DaemonRunner : Runner
{
private:

public:
   /**
    * Do we have to exit start?
    */
   static bool end;

   /**
    * Prepare a benchmark run
    * Loads the probes, allocates results table memory
    * \param pProbes list of probes to use
    * \param resultFileName file where to output the results
    * \param test the program to bench
    * \param args the args to pass to the program to bench
    * \param nbProcess the number of process that will be started
    * \param nbMetaRepet the number of meta repetition to run
    */
   DaemonRunner(ProbeList* pProbes, const std::string& resultFileName);

   /**
    */
   ~DaemonRunner();

   void start(unsigned int processNumber);
};

#endif
