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

#ifndef PROBEDATACOLLECTOR_HPP
#define PROBEDATACOLLECTOR_HPP

#include <pthread.h>

#include <vector>

#include "Probe.hpp"

class RunData;

class ProbeDataCollector
{
private:
    static const unsigned int MAX_BUFFER_SIZE = 500;

    pthread_t m_collectorThread;
    pthread_mutex_t m_mutex;
    bool m_threadRunning;

    ProbeList* m_pProbes; /*!< All probes to use */
    ProbeList m_periodicProbes;
    std::vector<RunData*> m_runData;

    unsigned int m_minPeriod; /** in µs */

public:
    ProbeDataCollector(ProbeList* pProbes);
    ~ProbeDataCollector();

    void allocateMemory();
    void clear();

    void start();
    void stop();

    void updateThread();

    size_t getNumberProbes()const { return m_pProbes->  size(); }
    RunData* getData(unsigned int index)const;
    const char* getLabel(unsigned int index)const;

    friend void* probeThread(void*);
};

#endif
