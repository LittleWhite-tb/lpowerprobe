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

#ifndef EXPERIMENTATIONRESULTSWRITER_HPP
#define EXPERIMENTATIONRESULTSWRITER_HPP

#include <fstream>
#include <string>

#include "ExperimentationResults.hpp"
#include "Probe.hpp"

/**
 * Class to write the \a ExperimentationResults in a file
 * The output format is a CSV containing all the data collected from \a ExperimentationResults
 * The first line is an header filled with the probes labels
 *
 * If the file could not be opened, the results will be outputted on standard output.
 */
class ExperimentationResultsWriter
{
private:
    std::ofstream m_outputFile; /*!< The file */

    /**
     * Writes the first line of the result file
     * This first line contains the labels returned by the probes
     * @param output the output stream to use
     * @param probes list of probes from where to get the labels
     */
    void writeHeader(std::ostream &output, const ProbeList& probes);

public:
    /**
     * Open the file
     * @param filePath the file to open
     */
    ExperimentationResultsWriter(const std::string& filePath);

    /**
     * Checks if the file is open
     * @return true if the file is open
     */
    bool isOpen()const { return m_outputFile.is_open(); }

    /**
     * Writes the CSV file using \a overheadResults, \a results and the list of probes \a probes
     * @param overheadResults
     * @param results
     * @param probes
     */
    void write(const ExperimentationResults& overheadResults, const ExperimentationResults& results, const ProbeList &probes);
};

#endif
