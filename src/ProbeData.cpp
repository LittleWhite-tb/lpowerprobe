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

#include "ProbeData.hpp"

#include <cassert>

ProbeData::ProbeData(unsigned int nbDevices, unsigned int nbChannels)
    :m_nbDevices(nbDevices),m_nbChannels(nbChannels),m_data(nbDevices, std::vector<double>(nbChannels,0))
{

}

double ProbeData::getValue(unsigned int device, unsigned int channel)const
{
    assert (device < m_nbDevices);
    assert (channel < m_nbDevices);

    return m_data[device][channel];
}

void ProbeData::setData(double* pData)
{
    for (unsigned int device = 0 ; device < m_nbDevices ; device++ )
    {
        for ( unsigned int channel = 0 ; channel < m_nbChannels ; channel++)
        {
            m_data[device][channel] = pData[device*m_nbChannels+channel];
        }
    }
}

std::ostream& operator<<(std::ostream& stream, const ProbeData& pd)
{
    for ( unsigned int device = 0 ; device < pd.getNbDevices() ; device++ )
    {
        for ( unsigned int channel = 0 ; channel < pd.getNbChannels() ; channel++ )
        {
            stream << pd.m_data[device][channel] << ";";
        }
    }

    return stream;
}

ProbeData& ProbeData::operator +=(const ProbeData& pd)
{
    assert(this->getNbDevices() == pd.getNbDevices());
    assert(this->getNbChannels() == pd.getNbChannels());

    for ( unsigned int device = 0 ; device < pd.getNbDevices() ; device++ )
    {
        for ( unsigned int channel = 0 ; channel < pd.getNbChannels() ; channel++ )
        {
            this->m_data[device][channel] += pd.m_data[device][channel];
        }
    }

    return *this;
}

ProbeData& ProbeData::operator /=(size_t quot)
{
    for ( unsigned int device = 0 ; device < this->getNbDevices() ; device++ )
    {
        for ( unsigned int channel = 0 ; channel < this->getNbChannels() ; channel++ )
        {
            this->m_data[device][channel] /= quot;
        }
    }

    return *this;
}

ProbeData* operator-(const ProbeData& pd1, const ProbeData& pd2)
{
    assert(pd1.getNbDevices() == pd2.getNbDevices());
    assert(pd1.getNbChannels() == pd2.getNbChannels());

    ProbeData* pPD = new ProbeData(pd1.getNbDevices(),pd2.getNbChannels());
    for ( unsigned int device = 0 ; device < pd1.getNbDevices() ; device++ )
    {
        for ( unsigned int channel = 0 ; channel < pd1.getNbChannels() ; channel++ )
        {
            pPD->m_data[device][channel] = pd1.m_data[device][channel] - pd2.m_data[device][channel];
        }
    }

    return pPD;
}
