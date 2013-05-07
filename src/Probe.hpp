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
#include <exception>

/**
 * @class ProbeLoadingException
 * @brief Thrown when a probe could not be loaded properly
 */
class ProbeLoadingException: public std::exception
{
   public:
   
      /**
       * \param message reason of the exception
       */
      ProbeLoadingException(const std::string& message)
         :message(message)
      {
      }
   
      virtual ~ProbeLoadingException(void)throw() {}

      /**
       * \return the littral reason of the exception
       */
      virtual const char* what() const throw()
      {
         return message.c_str();
      }
   
   private:
      std::string message;
};

/**
 * Wrapper around dynamic evaluation library
 */
class Probe
{
   typedef double (*evalGet)(void *data);
   typedef  void* (*evalInit)(void);
   typedef int (*evalClose)(void *data);
   
private:
   void* pLibHandle;
   void* pProbeHandle;

   evalGet evaluationStart;   
	evalGet evaluationStop;    
	evalInit evaluationInit;   
	evalClose evaluationClose;  

public:
   /**
    * Loads a probe from \a path
    * \param path the path to the file to load
    * \exception ProbeLoadingException on failure
    */
   Probe(const std::string& path);
   
   /**
    */
   ~Probe();

   /**
    * \return the actual value of the probe (start value)
    */
   double startMeasure();
   
   /**
    * \return the actual value of the probe (end value)
    */
   double stopMeasure();
};

typedef std::vector<Probe*> ProbeList; 

#endif
