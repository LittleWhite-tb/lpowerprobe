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

#ifndef RUNNER_HPP
#define RUNNER_HPP

#include <vector>
#include <fstream>

#include <semaphore.h>

#include "ProbeDataCollector.hpp"

class RunData;

/**
 * @brief The Runner class
 * @warning This class should be thread-safe
 */
class Runner
{
protected:

    ProbeDataCollector* m_pProbesDataCollector; /*!< Data collector for probes */

    unsigned int m_nbMetaRepet;/*!< Number of repetition to do */
    unsigned int m_nbProcess;  /*!< Number of process started */

    pid_t m_pid;   /*!< PID of the process to use as unique ID for semaphores */
    std::string m_pidString; /*!< PID as a string */
    sem_t* m_fatherLock;       /*!< Process test synchronisation */
    sem_t* m_processLock;      /*!< Process test synchronisation */
    sem_t* m_processEndLock;   /*!< Process end synchronisation */

public:

    /**
     * Constructor creating the semaphores for process locking
     * @param pProbesDataCollector the collector to use to get data from probes
     * @param nbProcess number of process that will be created
     * @param nbMetaRepet number of meta repeat that will be runned
     */
    Runner(ProbeDataCollector* pProbesDataCollector, unsigned int nbProcess, unsigned int nbMetaRepet);

    /**
     * Destroys the semaphores
     */
    virtual ~Runner();

    /**
     * Start the benchmark
     * \param pOverheadExpResult Arrays collecting the results of overhead run from the probes (using \a ProbeDataCollector)
     * \param pExpResult Arrays collecting the results run from the probes (using \a ProbeDataCollector)
     * \param processNumber the process number
     */
    virtual void start(ExperimentationResults* pOverheadExpResult, ExperimentationResults* pExpResult, unsigned int processNumber)=0;
};

#endif
