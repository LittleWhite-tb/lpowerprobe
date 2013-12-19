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

/**
 * Wrapper around probes data
 * The raw data coming from \a Probe are structures in a array of 1D with a size of nbDevice*nbChannels
 * This wrapper copy the data in a C++ like 2D array.
 * Moreover, several additional functions have been implemented to help applying computation of the data
 */
class ProbeData
{
private:
    unsigned int m_nbDevices; /*!< Number of devices to manage */
    unsigned int m_nbChannels; /*!< Number of channels to manage */

    std::vector< std::vector< double > > m_data; /*!< The data coming from \a Probe */

public:
    /**
     * Reserve the data storage for the incoming data from \a Probe
     * @param nbDevices Number of devices that the \a Probe handles
     * @param nbChannels Number of channels that the \a Probe handles
     * @todo Pass directly the \a Probe to the constructor as const ref
     */
    ProbeData(unsigned int nbDevices, unsigned int nbChannels);

    /**
     */
    ~ProbeData() {}

    /**
     * @return the number of devices that can handle \a ProbeData
     */
    unsigned int getNbDevices()const { return m_nbDevices; }

    /**
     * @return the number of channels that can handle \a ProbeData
     */
    unsigned int getNbChannels()const { return m_nbChannels; }

    /**
     * Get a specific data for \a device and \a channel
     * @param device the index of the device from which to get the data
     * @param channel the index of the channel from which to get the data
     * @return the data requested
     */
    double getValue(unsigned int device, unsigned int channel)const;

    /**
     * Sets new data in \a ProbeData
     * @param pData the raw data coming from the \a Probe
     */
    void setData(double* pData);

    /**
     * Aggregation operator to sum several \a PrbeData
     * @param pd the \a ProbeData to sum to the current
     * @return the sum
     */
    ProbeData& operator +=(const ProbeData& pd);

    /**
     * Divide all data in this \a ProbeData by \a quot
     * @param quot
     * @return the result
     */
    ProbeData& operator /=(size_t quot);

    /**
     * Difference between two \a ProbeData
     * @param pd1
     * @param pd2
     * @return a pointeur to a newly allocated ProbeData
     * @warning the pointer returned as to be freed
     */
    friend ProbeData* operator-(const ProbeData& pd1, const ProbeData& pd2);

    /**
     * Stream operator
     * @param stream
     * @param pd the \a ProbeData to write
     * @return a reference to the stream
     */
    friend std::ostream& operator<<(std::ostream& stream, const ProbeData& pd);
};

std::ostream& operator<<(std::ostream& stream, const ProbeData& pd);

#endif
