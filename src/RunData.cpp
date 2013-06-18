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

#include "RunData.hpp"

#include <cassert>

RunData::RunData(unsigned int maxBufferSize, unsigned int nbDevices, unsigned int nbChannels)
    :m_maxData(maxBufferSize),m_dataIndex(0),m_data(m_maxData,ProbeData(nbDevices,nbChannels))
{

}

const ProbeData& RunData::getProbeData(unsigned int index)const
{
    assert(index < m_dataIndex);

    return m_data[index];
}

bool RunData::needDump()const
{
    return m_dataIndex >= m_maxData;
}

void RunData::addValue(double* pData)
{
    assert(pData);

    m_data[m_dataIndex].setData(pData);
    m_dataIndex++;
}
