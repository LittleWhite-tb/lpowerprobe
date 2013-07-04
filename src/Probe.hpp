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

/**
 * Wrapper for probes
 * This class is virtual since we handles two different version of probes
 */
class Probe
{
private:

protected:
    void* pLibHandle;  /*!< Pointer to the probe library */
    void* pProbeHandle; /*!< Pointer to the probe library data */

    /**
     * Generic function to load a symbol from the probe library
     * @param symbol the name of the symbol
     * @return the pointer to the symbol
     */
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
    typedef double* (*libFini)(void *data); /*!< pointer to the function to close a probe library type */
    typedef  void* (*libInit)(void); /*!< pointer to the function to initialise a probe library type */

    libInit evaluationInit; /*!< probe initialisation function pointer */
    libFini evaluationFini; /*!< probe closure function pointer */

public:
    /**
     * Loads a probe library
     * @param path path to the probe library
     * @exception ProbeLoadingException thrown when the file could not be loaded
     */
    Probe(const std::string& path);

    /**
     * Calls the closure function \a evaluationFini if possible and close the probe library
     */
    virtual ~Probe();

    /**
     * Updates the probe
     */
    virtual void update();

    /**
     * Starts a new measurement with the probe
     */
    virtual void startMeasure()=0;

    /**
     * Stops the measurement of the probe
     * \return the actual value of the probe (end value)
     */
    virtual double* stopMeasure()=0;

    /**
     * Gets the probe version
     * @return
     */
    virtual unsigned int getVersion()const { return 1; }

    /**
     * Gets the number of devices handled by the probe
     * @return
     */
    virtual unsigned int getNbDevices()const { return 1; }

    /**
     * Gets the numbers of channels handled by the probe
     * @return
     */
    virtual unsigned int getNbChannels()const { return 1; }

    /**
     * Get the refresh period (step to call update) for this probe
     * @return
     */
    virtual unsigned int getPeriod()const { return 0; }

    /**
     * Get the label of the probe
     * The label is a string that will be used to describe the data returned in a human readable form
     * @return
     */
    virtual const char* getLabel()const { return ""; }
};
typedef std::vector<Probe*> ProbeList;

#endif
