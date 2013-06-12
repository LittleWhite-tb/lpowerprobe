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

#include "ProbeLoadingException.hpp"

ProbeV2::ProbeV2(const std::string& path)
{
    this->pLibHandle = dlopen(path.c_str(),RTLD_LAZY);
    if ( this->pLibHandle == NULL )
    {
       throw ProbeLoadingException("Error to load probe : '" + path + "' (" + dlerror() + ")");
    }

    unsigned int* pVersion = (unsigned int*)dlsym(this->pLibHandle,"version");
    if ( pVersion == NULL )
    {
        m_version = 1; // If the symbol is not found, we have certainly the old lib version
        throw ProbeLoadingException("Error to load 'version' in probe : '" + path + "'");
    }
    m_version = *pVersion;
    if ( m_version != 2 )
    {
        throw ProbeLoadingException("Incompatible probe version (required : 2)");
    }


    unsigned int* pPeriod = (unsigned int*)dlsym(this->pLibHandle,"period");
    if ( pPeriod == NULL )
    {
        throw ProbeLoadingException("Error to load 'period' in probe : '" + path + "'");
    }
    m_period = *pPeriod;

    this->m_pLabel = (const char*)dlsym(this->pLibHandle,"label");
    if ( this->m_pLabel == NULL )
    {
        throw ProbeLoadingException("Error to load 'label' in probe : '" + path + "'");
    }

    this->evaluationInit =(libInit) dlsym(this->pLibHandle,"init");
    if ( this->evaluationInit == NULL )
    {
        throw ProbeLoadingException("Error to load 'init' in probe : '" + path + "'");
    }

    this->evaluationStart =(libStart) dlsym(this->pLibHandle,"start");
    if ( this->evaluationStart == NULL )
    {
        throw ProbeLoadingException("Error to load 'start' in probe : '" + path + "'");
    }

    this->evaluationStop = (libStop) dlsym(this->pLibHandle,"stop");
    if ( this->evaluationStop == NULL )
    {
        throw ProbeLoadingException("Error to load 'stop' in probe : '" + path + "'");
    }

    this->evaluationFini = (libFini) dlsym(this->pLibHandle,"fini");
    if ( this->evaluationFini == NULL )
    {
       throw ProbeLoadingException("Error to load 'fini' in probe : '" + path + "'");
    }

    this->evaluationFini = (libFini) dlsym(this->pLibHandle,"fini");
    if ( this->evaluationFini == NULL )
    {
       throw ProbeLoadingException("Error to load 'fini' in probe : '" + path + "'");
    }

    this->evaluationGetNbDevices = (libGetNbDevices) dlsym(this->pLibHandle,"nbDevices");
    if ( this->evaluationGetNbDevices == NULL )
    {
       throw ProbeLoadingException("Error to load 'nbDevices' in probe : '" + path + "'");
    }

    this->evaluationGetNbChannels = (libGetNbChannels) dlsym(this->pLibHandle,"nbChannels");
    if ( this->evaluationGetNbChannels == NULL )
    {
       throw ProbeLoadingException("Error to load 'nbChannels' in probe : '" + path + "'");
    }

    std::cout << "'" << path << "' successfully loaded" << std::endl;

    // Now, we just start the probe
    if ( this->evaluationInit )
    {
       this->pProbeHandle = this->evaluationInit();
    }
}

ProbeV2::~ProbeV2()
{
    this->evaluationFini(this->pProbeHandle);

    dlclose(this->pLibHandle); // Should return zero
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
    return this->evaluationGetNbDevices();
}

unsigned int ProbeV2::getNbChannels()const
{
    return this->evaluationGetNbChannels();
}
