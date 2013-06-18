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


#ifndef RUNDATA_HPP
#define RUNDATA_HPP

#include <vector>

#include "ProbeData.hpp"

class RunData
{
private:
    unsigned int m_maxData;
    unsigned int m_dataIndex;

    std::vector< ProbeData > m_data;

public:
    RunData(unsigned int maxBufferSize, unsigned int nbDevices, unsigned int nbChannels);
    ~RunData() {}

    unsigned int getNbData()const { return m_dataIndex; }

    const ProbeData& getProbeData(unsigned int index)const;

    bool needDump()const;
    void addValue(double* pData);

    void reinit() { m_dataIndex = 0; }
};

#endif
