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

#ifndef EXPERIMENTATION_HPP
#define EXPERIMENTATION_HPP

#include <vector>
#include <string>

#include "ExperimentationResults.hpp"
#include "Probe.hpp"

class Runner;
class ProbeDataCollector;
class Options;

/**
 * Set up the experimentation context and start the benchmarking using \a Runner
 * The experimentation is following the following path :
 * - Looping for all experimentation
 * - - Looping for all process
 * - - - Creating a thread
 * - - - Looping for all metarepetition (Runner)
 * - - - Start test program (Runner)
 * 
 * The forks gets an higher priority and can be pinned 
 */
class Experimentation
{
protected:
   ProbeList m_probes;  /*!< Probe to use during a test */
   ExperimentationResults* m_pOverheadResults; /*!< \a Probe data for the overhead calculation */
   ExperimentationResults* m_pResults; /*!< \a Probe data for the benchmark */
   ProbeDataCollector* m_pProbeDataCollector; /*!< Collector in order to get the \a ProbeData */

   const Options& m_options;  /*!< Set of option comming from program args */
   std::string m_execFile; /*!< File to run (either a compiled kernel of a compiled program) */
   
   std::vector<std::string> m_probePaths; /*!< List of probes to load */

   /**
    * Saves the results collected in a file
    * The file is specified by the user through \a Options
    */
   void saveResults();

public:
   /**
    * Loads and prepares the probes to use for the experimentation
    * Prepares the memory space for the results
    * \param options the options given by the user
    * \exception ProbeLoadingException when no probes could be loaded
    */
   Experimentation(const Options& options);
   
   /**
    */
   virtual ~Experimentation();
   
   /**
    * Starts the experimentation process
    */
   virtual void start()=0;
};

#endif
