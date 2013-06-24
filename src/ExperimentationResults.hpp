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

class ExperimentationResults
{
private:

    std::vector<RunData> m_results; /*!< Results contained in a vector of meta repeat size */
    unsigned int m_nbMeasurement;
public:

    ExperimentationResults(unsigned int nbMetaRepeat, const ProbeList& probes);

    bool isFull()const { return m_nbMeasurement >= m_results.size(); }
    void extend(const ProbeList& probes);

    void setProbeData(unsigned int probeIndex, double* pData);
    void measurementDone() { m_nbMeasurement++; }

    const std::vector<RunData>& getResults()const { return m_results; }

};

#endif
