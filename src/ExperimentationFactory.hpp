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

#ifndef EXPERIMENTATIONFACTORY_HPP
#define EXPERIMENTATIONFACTORY_HPP

#include "Options.hpp"

class Experimentation;

/**
 * Factory pattern to create an \a Experimentation
 */
class ExperimentationFactory
{
private:

public:
    /**
     * Create a new \a Experimentation
     * @param options the options to pass to the new \a Experimentation
     * @return a pointer on the \a Experimentation
     * @warning You have to free the pointer returned
     */
    static Experimentation* createExperimentation(const Options& options);
};

#endif
