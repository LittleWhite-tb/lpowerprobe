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

#ifndef EXPERIMENTATIONRESULTSWRITER_HPP
#define EXPERIMENTATIONRESULTSWRITER_HPP

#include <fstream>
#include <string>

#include "ExperimentationResults.hpp"
#include "Probe.hpp"

class ExperimentationResultsWriter
{
private:
    std::ofstream m_outputFile;

    void writeHeader(std::ostream &output, const ProbeList& probes);

public:
    ExperimentationResultsWriter(const std::string& filePath);

    bool isOpen()const { return m_outputFile.is_open(); }
    void write(const ExperimentationResults& overheadResults, const ExperimentationResults& results, const ProbeList &probes);
};

#endif
