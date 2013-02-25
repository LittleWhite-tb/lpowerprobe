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
 
#ifndef CPUUTILS_HPP
#define CPUUTILS_HPP

/**
 * Utility class to control CPU behavior
 * Thanks to this class you can pin your current process to a specific
 * core using \a pinCPU() and set the FIFO priority with \a setFifoMaxPriority()
 */
class CPUUtils
{
private:

public:
   /**
    * Pin the current process to a specific CPU core
    * \param cpuID the id or the core
    */
   static void pinCPU(int cpuID);
   
   /**
    * Sets a FIFO priority for the current process
    * If the \a offset argument is 0, the maximal priority is set. The priority
    * will be set to MAX + offset. So offset should be < 0.
    * \param offset an offset to not set the max priority
    */
   static void setFifoMaxPriority(int offset);
};

#endif
