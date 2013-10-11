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

#ifndef PROBEV2_HPP
#define PROBEV2_HPP

#include "Probe.hpp"

#include <string>

/**
 * Wrapper around dynamic evaluation library version 2
 */
class ProbeV2 : public Probe
{
    typedef unsigned int (*libGetNbDevices)(void *data);
    typedef unsigned int (*libGetNbChannels)(void *data);

    typedef void (*libStart)(void *data);
    typedef double* (*libStop)(void *data);
    typedef void (*libUpdate)(void *data);

private:

    unsigned int m_version;
    unsigned int m_period;
    const char* m_pLabel;

    libUpdate evaluationUpdate;

    libStart evaluationStart;
    libStop evaluationStop;

    libGetNbDevices evaluationGetNbDevices;
    libGetNbChannels evaluationGetNbChannels;

public:
    /**
     * Loads a probe from \a path
     * \param path the path to the file to load
     * \exception ProbeLoadingException on failure
     */
   ProbeV2(const std::string& path);

   /**
    */
   ~ProbeV2() {}

   void update();

   void startMeasure();
   double* stopMeasure();

   unsigned int getVersion()const { return m_version; }

   unsigned int getNbDevices()const;
   unsigned int getNbChannels()const;

   unsigned int getPeriod()const { return m_period; }
   const char* getLabel()const { return m_pLabel; }
};

#endif
