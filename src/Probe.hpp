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

#ifndef PROBE_HPP
#define PROBE_HPP

#include <string>
#include <vector>
#include <cassert>

#include <dlfcn.h>

#include "ProbeLoadingException.hpp"

class Probe
{
private:

protected:
    void* pLibHandle;
    void* pProbeHandle;

    template <typename T>
    T loadSymbol(const char* symbol)
    {
        assert(symbol);

       T pSymbol = (T)dlsym(this->pLibHandle,symbol);
       if ( pSymbol == NULL )
       {
           throw ProbeLoadingException(std::string("Error to load '") + symbol + "' (" + dlerror() + ")");
       }

       return pSymbol;
    }

protected:
    typedef double* (*libFini)(void *data);
    typedef  void* (*libInit)(void);

    libInit evaluationInit;
    libFini evaluationFini;

public:
    Probe(const std::string& path);
    virtual ~Probe();

    virtual void update();

    /**
     */
    virtual void startMeasure()=0;

    /**
     * \return the actual value of the probe (end value)
     */
    virtual double* stopMeasure()=0;

    virtual unsigned int getVersion()const { return 1; }

    virtual unsigned int getNbDevices()const { return 1; }
    virtual unsigned int getNbChannels()const { return 1; }

    virtual unsigned int getPeriod()const { return 0; }
    virtual const char* getLabel()const { return ""; }
};
typedef std::vector<Probe*> ProbeList;

#endif
