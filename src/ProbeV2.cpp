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

#include "ProbeV2.hpp"

#include <iostream>
#include <dlfcn.h>

#include "InvalidProbeVersionException.hpp"
#include "ProbeInitialisationException.hpp"

ProbeV2::ProbeV2(const std::string& path)
    :Probe(path)
{
    unsigned int* pVersion = NULL;
    try
    {
      pVersion = loadSymbol<unsigned int*>("version");
    }
    catch (ProbeLoadingException& ple )
    {
       // Ok, we failed to find the symbol, it's maybe version 1
       throw InvalidProbeVersionException("Incompatible probe version (version symbol not found)");
    }
    
    // At least, pVersion is not NULL
    m_version = *pVersion;
    if ( m_version != 2 )
    {
        throw InvalidProbeVersionException("Incompatible probe version (required : 2)");
    }

    unsigned int* pPeriod = loadSymbol<unsigned int*>("period");
    m_period = *pPeriod;

    this->m_pLabel = *loadSymbol<const char**>("label");

    this->evaluationInit =loadSymbol<libInit>("init");
    this->evaluationFini = loadSymbol<libFini>("fini");

    this->evaluationStart = loadSymbol<libStart>("start");
    this->evaluationUpdate = loadSymbol<libUpdate>("update");
    this->evaluationStop = loadSymbol<libStop>("stop");

    this->evaluationGetNbDevices = loadSymbol<libGetNbDevices>("nbDevices");
    this->evaluationGetNbChannels = loadSymbol<libGetNbChannels>("nbChannels");

    std::cout << "'" << path << "' successfully loaded" << std::endl;

    // Now, we just start the probe
    if ( this->evaluationInit )
    {
        this->pProbeHandle = this->evaluationInit();
        if ( this->pProbeHandle == NULL )
        {
            throw ProbeInitialisationException(path);
        }
    }
}

void ProbeV2::update()
{
    this->evaluationUpdate(this->pProbeHandle);
}

void ProbeV2::startMeasure()
{
    this->evaluationStart(this->pProbeHandle);
}

double* ProbeV2::stopMeasure()
{
    return this->evaluationStop(this->pProbeHandle);
}

unsigned int ProbeV2::getNbDevices()const
{
    return this->evaluationGetNbDevices(this->pProbeHandle);
}

unsigned int ProbeV2::getNbChannels()const
{
    return this->evaluationGetNbChannels(this->pProbeHandle);
}
