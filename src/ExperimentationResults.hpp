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

#ifndef EXPERIMENTATIONRESULTS_HPP
#define EXPERIMENTATIONRESULTS_HPP

#include <vector>

#include "RunData.hpp"

/**
 * Storage for all the data of an experimentation
 * Wrapper around an array of \a RunData
 * Since we needs to handle a unknown number of run, the functoin \a extend() allows to double the size (provoquing an allocation).
 * To avoid this allocation, the \a ExperimentationResults is prepared following the information passed to the constructor.
 */
class ExperimentationResults
{
private:

    std::vector<RunData> m_results; /*!< Results contained in a vector of meta repeat size */
    unsigned int m_nbMeasurement; /*!< Number of measurement done for this \a Experimentation */
public:

    /**
     * Prepares the storage for the \a Experimentation data
     * @param nbMetaRepeat
     * @param probes
     */
    ExperimentationResults(unsigned int nbMetaRepeat, const ProbeList& probes);

    /**
     * Checks if the probe data array is full (and needs to be extended with \a extend() next time a new data is pushed with \a setProbeData())
     * @return true when the array is full and requires a new memomry allocation to keep more data
     */
    bool isFull()const { return m_nbMeasurement >= m_results.size(); }

    /**
     * Increase the space allocated to store the \a Experimentation data
     * @param probes the list of probes from which one the data will be collected
     */
    void extend(const ProbeList& probes);

    /**
     * Set new data for a specific probe \a probeIndex
     * @param probeIndex the index of the probe from which one the data are coming
     * @param pData the pointeur to the raw data
     */
    void setProbeData(unsigned int probeIndex, double* pData);

    /**
     * Notify \a ExperimentationResults that a measurement is finished and
     * so all following data will be set for the next measurement
     * After calling that, it is advice to check the fullness of the array with \a isFull in order to see if an additionnal allocation is needed
     */
    void measurementDone() { m_nbMeasurement++; }

    /**
     * Get the results
     * @return
     */
    const std::vector<RunData>& getResults()const { return m_results; }

    /**
     * Get the size of the results (some extra data at the end of the vector must be ignored).
     *
     * @return The number of valid results in the results.
     */
    unsigned int getNbResults() const { return m_nbMeasurement; }

};

#endif
