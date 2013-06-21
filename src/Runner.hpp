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
#include <fstream>

#include <semaphore.h>

#include "ProbeDataCollector.hpp"

class RunData;

class Runner
{
protected:

    std::ofstream m_resultFile;   /*!< */

    ProbeDataCollector* m_pProbesDataCollector; /*!< Data collector for probes */

    unsigned int m_nbMetaRepet;/*!< Number of repetition to do */
    unsigned int m_nbProcess;  /*!< Number of process started */

    // Contains results for
    // - All meta repet
    // - All probes
    typedef std::vector < std::vector<std::vector< RunData* > > > GlobalResultsArray;
    GlobalResultsArray m_overheadResults;  /*!< Probe results for the overhead test */
    GlobalResultsArray m_runResults; /*!< Probe results for the run test */

    pid_t m_pid;   /*!< PID of the process to use as unique ID for semaphores */
    std::string m_pidString; /*!< PID as a string */
    sem_t* m_fatherLock;       /*!< Process test synchronisation */
    sem_t* m_processLock;      /*!< Process test synchronisation */
    sem_t* m_processEndLock;   /*!< Process end synchronisation */

    /**
     * Saves the result in a file after applying the overhead bias
     */
    void saveResults();

public:

    Runner(ProbeDataCollector* pProbesDataCollector, const std::string& resultFileName, unsigned int nbProcess, unsigned int nbMetaRepet);
    virtual ~Runner();

    /**
     * Start the benchmark
     * \param processNumber the process number
     */
    virtual void start(unsigned int processNumber)=0;
};

#endif
