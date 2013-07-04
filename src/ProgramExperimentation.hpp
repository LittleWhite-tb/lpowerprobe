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

#ifndef PROGRAMEXPERIMENTATION_HPP
#define PROGRAMEXPERIMENTATION_HPP

#include "Experimentation.hpp"

#include "Options.hpp"

/**
 * Exerimentation implementation for executable benchmarking
 */
class ProgramExperimentation : public Experimentation
{
private:

public:

    /**
     * @param options the options to use for the \a Experimentation
     */
    ProgramExperimentation(const Options& options);

    /**
     */
    ~ProgramExperimentation() {}

    void start();

    /**
     * @brief runnerThread the thread running the program
     * @param pArgs a pointer to \a ProgramExperimentation
     * @return NULL
     */
    friend void* runnerThread(void* pArgs);
};

#endif
