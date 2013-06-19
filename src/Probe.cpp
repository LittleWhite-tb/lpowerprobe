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

#include <exception>
#include <stdexcept>
#include <cassert>

#include <dlfcn.h>

#include "ProbeLoadingException.hpp"
/*
template <typename T>
T Probe::loadSymbol(const char* symbol)
{
    assert(symbol);

    T pSymbol = (T)dlsym(this->pLibHandle,symbol);
    if ( pSymbol == NULL )
    {
        throw ProbeLoadingException(std::string("Error to load '") + symbol + std::string("'"));
    }

    return pSymbol;
}*/

Probe::Probe(const std::string& path)
    :evaluationInit(NULL),evaluationFini(NULL)
{
    this->pLibHandle = dlopen(path.c_str(),RTLD_LAZY);
    if ( this->pLibHandle == NULL )
    {
      throw ProbeLoadingException("Error to load probe : '" + path + "' (" + dlerror() + ")");
    }
}

Probe::~Probe()
{
    if ( this->evaluationFini )
    {
        this->evaluationFini(this->pProbeHandle);
    }

    dlclose(this->pLibHandle); // Should return zero
}

void Probe::update()
{
    throw std::runtime_error("Probe::update() not available for probe version 1");
}
