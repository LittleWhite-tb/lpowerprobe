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

#include <iostream>
#include <cstdlib>
#include "Probe.hpp"

#include <exception>
#include <stdexcept>
#include <cassert>

#include <dlfcn.h>

#include "ProbeLoadingException.hpp"
#include "ProbeInitialisationException.hpp"

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

Probe::Probe(const std::string& path, const char *initName,
             const char *finiName) : 
   pProbeHandle (NULL),
   m_name (path),
   is_initialized (false),
   evaluationInit(NULL),
   evaluationFini(NULL)
{
    this->pLibHandle = dlopen(path.c_str(),RTLD_LAZY);
    if ( this->pLibHandle == NULL )
    {
      throw ProbeLoadingException(std::string(dlerror()));
    }

    this->evaluationInit = loadSymbol <libInit>(initName, MANDATORY);
    this->evaluationFini = loadSymbol <libFini>(finiName, MANDATORY);
}

Probe::~Probe()
{
   // Fini library callback
   this->fini ();

   // Library close
   int ret = dlclose (this->pLibHandle);
   this->pLibHandle = NULL;

   // Library close error checking
   if (ret != 0) {
      std::cerr << "An error occured while closing the library: " << std::endl
                   << dlerror () << std::endl;
      exit (EXIT_FAILURE);
   }
}

void Probe::update()
{
    throw std::runtime_error("Probe::update() not available for probe version 1");
}

void Probe::init () {
   assert (this->evaluationInit != NULL);
   this->pProbeHandle = this->evaluationInit ();
   this->is_initialized = true;
   
   // Give the user some feedback
   std::cerr << m_name << " successfully loaded and initialized" << std::endl;
}

void Probe::fini () {
   assert (this->evaluationFini != NULL);
   if (this->is_initialized) {
      this->evaluationFini (this->pProbeHandle);
   }
   this->is_initialized = false;
   this->pProbeHandle = NULL;
}
