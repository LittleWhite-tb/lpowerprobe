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

#include "ProbeDataCollector.hpp"

#include <iostream>
#include <exception>
#include <stdexcept>
#include <cassert>
#include <climits>

#include <ctime>

#include "RunData.hpp"

void* probeThread(void* args)
{
    ProbeDataCollector* pProbeDataCollector = reinterpret_cast<ProbeDataCollector*>(args);
    pProbeDataCollector->updateThread();

    return NULL;
}

int gcd(int val1, int val2)
{
    int ret = 0;

    if (val1 < val2)
        std::swap(val1, val2);

    ret = val1 % val2;

    if (ret == 0)
        return val2;
    else
        return gcd (val2, ret);
}

void ProbeDataCollector::allocateMemory()
{
    // ¨Prepare the memory space for probes
    for (size_t i = 0 ; i < m_pProbes->size() ; i++ )
    {
        unsigned int nbDevices = (*m_pProbes)[i]->getNbDevices();
        unsigned int nbChannels = (*m_pProbes)[i]->getNbChannels();
        unsigned int bufferSize = 1;
        /*
        if ( m_pProbes[i]->getPeriod() > 0 )
        {
            bufferSize = ProbeDataCollector::MAX_BUFFER_SIZE;
        }
        */

        m_runData[i] = new RunData(bufferSize,nbDevices,nbChannels);
    }
}

void ProbeDataCollector::clear()
{
    m_runData.clear();
}

ProbeDataCollector::ProbeDataCollector(ProbeList* pProbes)
    :m_threadRunning(true),m_pProbes(pProbes),m_runData(pProbes->size()),m_minPeriod(INT_MAX)
{
    assert(pProbes);

    bool needThread = false;
    // Get the highest update period required
    for (ProbeList::const_iterator itProbe = m_pProbes->begin() ; itProbe != m_pProbes->end() ; ++itProbe)
    {
        unsigned int period = (*itProbe)->getPeriod();
        if ( period != 0 )
        {
            m_periodicProbes.push_back(*itProbe);
            if ( m_minPeriod != INT_MAX )
            {
                m_minPeriod = gcd(m_minPeriod,period);
            }
            else // First time here
            {
                m_minPeriod = period;
            }
            needThread = true;
        }
    }

    // Create the update thread
    if (needThread)
    {
        pthread_attr_t attr;
        if (pthread_attr_init(&attr) != 0 )
        {
            std::cerr << "Failed to init attributes for thread" << std::endl;
            throw std::runtime_error("pthread_attr_init");
        }
        if ( pthread_create(&m_collectorThread,&attr,probeThread,this) != 0 )
        {
            std::cerr << "Failed to create collector thread" << std::endl;
            throw std::runtime_error("pthread_create");
        }

        if ( pthread_mutex_init(&m_mutex,NULL) != 0 )
        {
            std::cerr << "Failed to create mutex for collector thread" << std::endl;
            throw std::runtime_error("pthread_mutex_init");
        }
    }
}

ProbeDataCollector::~ProbeDataCollector()
{
    m_threadRunning = false;
    if ( pthread_cancel(m_collectorThread) != 0 )
    {
        std::cerr << "Failed to cancel collector thread" << std::endl;
        throw std::runtime_error("pthread_cancel");
    }

    for (unsigned int i = 0 ; i < m_runData.size() ; i++ )
    {
        delete m_runData[i];
    }

    if ( pthread_mutex_destroy(&m_mutex) != 0 )
    {
        std::cerr << "Failed to destroy mutex" << std::endl;
        throw std::runtime_error("pthread_mutex_destroy");
    }
}

void ProbeDataCollector::start()
{
    for (ProbeList::const_iterator itProbe = m_pProbes->begin() ; itProbe != m_pProbes->end() ; ++itProbe)
    {
        (*itProbe)->startMeasure();
    }

    pthread_mutex_unlock(&m_mutex);
}

void ProbeDataCollector::stop()
{
    for (unsigned int i = 0 ; i < m_pProbes->size() ; i++)
    {
        m_runData[i]->addValue((*m_pProbes)[i]->stopMeasure());
    }

    pthread_mutex_lock(&m_mutex);
}

void ProbeDataCollector::updateThread()
{
    long unsigned int elapsedTime = 0; /* in µs */
    timespec ts;
    ts.tv_sec = m_minPeriod/1000000;
    ts.tv_nsec = (m_minPeriod%1000000) *1000;

    while ( m_threadRunning )
    {
        pthread_mutex_lock(&m_mutex);

        nanosleep(&ts,NULL);
        elapsedTime += m_minPeriod;

        for (unsigned int i = 0 ; i < m_periodicProbes.size() ; i++)
        {
            unsigned int period = m_periodicProbes[i]->getPeriod();
            long int stepTime = elapsedTime % period;
            long int relativeTime = stepTime - period;
            if ( relativeTime < 5 && relativeTime > -5 )
            {
                m_periodicProbes[i]->update();
            }
        }

        pthread_mutex_unlock(&m_mutex);
    }
}

RunData* ProbeDataCollector::getData(unsigned int index)const
{
    assert(index < m_pProbes->size());

    return m_runData[index];
}
const char* ProbeDataCollector::getLabel(unsigned int index)const
{
    assert(index < m_pProbes->size());

    return (*m_pProbes)[index]->getLabel();
}
