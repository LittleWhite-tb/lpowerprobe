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

#ifndef PROBEDATACOLLECTOR_HPP
#define PROBEDATACOLLECTOR_HPP

#include <pthread.h>

#include <vector>

#include "Probe.hpp"
#include "ExperimentationResults.hpp"

class RunData;

/**
 * Class to connect the data from the probes
 */
class ProbeDataCollector
{
private:
    static const unsigned int MAX_BUFFER_SIZE = 500;

    pthread_t m_collectorThread;    /*!< thread updating periodically the probes (if needed) */
    pthread_cond_t m_cond;        /*!< condition to lock collector thread */
    pthread_mutex_t m_mutex;        /*!< mutex for the condition variable */
    bool m_needThread;              /*!< flag to know if a thread is updated */
    volatile bool m_threadCollecting;           /*!< flag to know if the thread is collecting data (when false, the thread will pause) */
    volatile bool m_threadRunning;           /*!< flag to know if the thread is running (when false, the thread should quit) */

    ProbeList* m_pProbes; /*!< All probes to use */
    ProbeList m_periodicProbes; /*!< probes needing a periodic update */

    unsigned int m_minPeriod; /** in µs */

public:
    /**
     * @param pProbes Pointer on the list of probes to collect
     */
    ProbeDataCollector(ProbeList* pProbes);

    /**
     */
    ~ProbeDataCollector();

    /**
     * Start the probes to monitor
     */
    void start();

    /**
     * Stop the probes to monitor
     * @param pResults the \a ExperimentationResults to fill
     * @todo see if factorisation is possible with \a cancel()
     */
    void stop(ExperimentationResults* pResults);

    /**
     * Stops the updating thread
     */
    void cancel();

    /**
     * Function run by the updating thread, going through the \a m_periodicProbes to update these
     */
    void updateThread();

    /**
     * Return the probes monitored
     * @return
     */
    const ProbeList& getProbes()const { return *m_pProbes; }

    /**
     * Get the number of probes monitored
     * @return
     */
    size_t getNumberProbes()const { return m_pProbes->size(); }

    /**
     * Get the label for a specific probe
     * @param index the index of the probe
     * @return the label as a human readable string
     */
    const char* getLabel(unsigned int index)const;

    /**
     * The function used to start the thread
     * It will call \a updateThread()
     * @return NULL
     */
    friend void* probeThread(void*);
};

#endif
