/*
 * lPowerProbe - A light benchmark tool oriented for energy probing
 *               heavely based on likwid
 * Copyright (C) 2013-2014 Universite de Versailles
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

#ifndef EXPERIMENTATIONTHREADARGS_HPP
#define EXPERIMENTATIONTHREADARGS_HPP

/**
 * Structure to keep the arguments transferred in Experimentation implementation thread
 */
struct ExperimentationThreadArgs
{
    Experimentation* pExp; /*!< The \a Experimentation throwing the thread using these arguments */
    Runner* pRunner;/*!< The \a Runner to use to run the benchmark */
    unsigned int threadNumber; /*!< thread number */

    /**
     * Constructor to fill the structure
     * @param pExp
     * @param pRunner
     * @param threadNumber
     */
    ExperimentationThreadArgs(Experimentation* pExp, Runner* pRunner, unsigned int threadNumber)
        :pExp(pExp),pRunner(pRunner),threadNumber(threadNumber) {}
};

#endif
