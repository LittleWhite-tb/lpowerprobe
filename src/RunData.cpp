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

RunData::RunData(const ProbeList &probes)
{
    for ( unsigned int i = 0 ; i < probes.size() ; i++ )
    {
        m_data.push_back(ProbeData(probes[i]->getNbDevices(),probes[i]->getNbChannels()));
    }
}

const ProbeData& RunData::getProbeData(unsigned int index)const
{
    assert(index < getNbProbeData());

    return m_data[index];
}

void RunData::setValue(unsigned int index, double* pData)
{
    assert(index < getNbProbeData());

    m_data[index].setData(pData);
}
