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


#ifndef RUNDATA_HPP
#define RUNDATA_HPP

#include <vector>

#include "Probe.hpp"
#include "ProbeData.hpp"

/**
 * \a ProbeData Array wrapper for each probes
 */
class RunData
{
private:
    std::vector< ProbeData > m_data; /*!< Vector sized with the number of probe */

public:
    /**
     * Contructs the array following the number of probes in \a ProbeList
     * @param probes the list of probes
     */
    RunData(const ProbeList& probes);

    /**
     */
    ~RunData() {}

    /**
     * @return Gets the number of probes
     */
    size_t getNbProbeData()const { return m_data.size(); }

    /**
     * @return Gets the whole array of data
     */
    const std::vector<ProbeData>& getData()const { return m_data; }

    /**
     * @param index index of the probe from which you want the data
     * @return Gets a specific \a ProbeData following the index of the Probe
     */
    const ProbeData& getProbeData(unsigned int index)const;

    /**
     * @brief Sets new data from a specific probe at \a index
     * @param index index of the probe from which the data are
     * @param pData the pointer of raw probe data
     */
    void setValue(unsigned int index, double* pData);
};

#endif
