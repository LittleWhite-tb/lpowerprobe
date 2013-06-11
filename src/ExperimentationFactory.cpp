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

#include "ExperimentationFactory.hpp"

#include "ProgramExperimentation.hpp"
#include "KernelExperimentation.hpp"

Experimentation* ExperimentationFactory::createExperimentation(const Options& options)
{
    if ( options.isExecKernel() )
    {
        return new KernelExperimentation(options);
    }
    else
    {
        return new ProgramExperimentation(options);
    }
}
