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


#ifndef PROBEDATA_HPP
#define PROBEDATA_HPP

#include <vector>
#include <iostream>

#include <cstddef>

class ProbeData
{
private:
    unsigned int m_nbDevices;
    unsigned int m_nbChannels;

    std::vector< std::vector< double > > m_data;

public:
    ProbeData(unsigned int nbDevices, unsigned int nbChannels);
    ~ProbeData() {}

    unsigned int getNbDevices()const { return m_nbDevices; }
    unsigned int getNbChannels()const { return m_nbChannels; }

    double getValue(unsigned int device, unsigned int channel)const;
    void setData(double* pData);

    ProbeData& operator +=(const ProbeData& pd);
    ProbeData& operator /=(size_t quot);

    friend ProbeData* operator-(const ProbeData& pd1, const ProbeData& pd2);

    friend std::ostream& operator<<(std::ostream& stream, const ProbeData& pd);
};

std::ostream& operator<<(std::ostream& stream, const ProbeData& pd);

#endif
