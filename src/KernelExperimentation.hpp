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

#ifndef KERNELEXPERIMENTATION_HPP
#define KERNELEXPERIMENTATION_HPP

#include "Experimentation.hpp"

#include "Options.hpp"

/**
 * Implementation for kernel experimentation
 * The kernel is a small program that should be compiled with \a KernelCompiler in order to get run
 */
class KernelExperimentation : public Experimentation
{
private:
    static const std::string DUMMY_KERNEL_FILE;  /*!< Name of the dummy kernel for overload calculation */

    std::string m_dummyKernelFile;   /*!< Dummy kernel for overload calculation */

public:
    /**
     * @brief KernelExperimentation
     * @param options
     */
    KernelExperimentation(const Options& options);

    /**
     */
    ~KernelExperimentation() {}

    /**
     */
    void start();

    /**
     * Thread to start the runner
     * @param pArgs user args (pointer \a KernelExperimentation)
     * @return NULL
     */
    friend void* kernelRunnerThread(void* pArgs);
};

#endif
