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

#include "Probe.hpp"
#include "ProbeV2.hpp"

#include <iostream>
#include <dlfcn.h>

#include "ProbeInitialisationException.hpp"

ProbeV2::ProbeV2(const std::string& path)
    :Probe(path, "init", "fini")
{
    unsigned int* pVersion = NULL;
    pVersion = loadSymbol<unsigned int*>("version", MANDATORY);
   
    // At least, pVersion is not NULL
    m_version = *pVersion;
    if ( m_version != 2 )
    {
        throw ProbeLoadingException("Incompatible probe version (required : 2)");
    } 

    this->m_pLabel = *loadSymbol<const char**>("label", FACULTATIVE);
 
    // Load libraries
    this->evaluationStart = loadSymbol<libStart>("start", MANDATORY);
    this->evaluationStop = loadSymbol<libStop>("stop", MANDATORY);
    this->evaluationUpdate = loadSymbol<libUpdate>("update", FACULTATIVE);
    this->evaluationGetNbDevices = loadSymbol<libGetNbDevices>("nbDevices", FACULTATIVE);
    this->evaluationGetNbChannels = loadSymbol<libGetNbChannels>("nbChannels", FACULTATIVE);
    unsigned int* pPeriod = loadSymbol<unsigned int*>("period", FACULTATIVE);

    // Check errors considering facultative symbols 
    if (pPeriod == NULL) {
       m_period = 0;
    } else {
       m_period = *pPeriod;
    }
    
    if (this->evaluationUpdate == NULL && this->m_period != 0) {
      throw  ProbeLoadingException (std::string ("Error: A period is defined but there is no update function.\n"));
    }

    // Now, we just init the probe (callback to init)
    this->init ();
}

void ProbeV2::update()
{
   assert (this->evaluationUpdate != NULL);
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
   if (this->evaluationGetNbDevices != NULL) {
      return this->evaluationGetNbDevices(this->pProbeHandle);
   } else {
      return 1;
   }
}

unsigned int ProbeV2::getNbChannels()const
{
   if (this->evaluationGetNbChannels != NULL) {
      return this->evaluationGetNbChannels(this->pProbeHandle);
   } else {
      return 1;
   }
}
