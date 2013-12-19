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
 
#include "Experimentation.hpp"

#include "ProbeLoadingException.hpp"
#include "ProbeLoader.hpp"
#include "ProbeDataCollector.hpp"
#include "ExperimentationResultsWriter.hpp"

#include "Options.hpp"

Experimentation::Experimentation(const Options& options)
   :m_options(options),m_execFile(options.getExecName())
{
   ProbeLoader pl;
   pl.addDirectory(options.getProgramPath());
   pl.loadProbes(options.getProbesPath(),m_probes); // Can return error, but we can try to run
   if ( m_probes.size() == 0 )
   {
      // No probes ... :(
      throw ProbeLoadingException("No probes loaded");
   }

   m_pOverheadResults = new ExperimentationResults(options.getNbRepetition(),m_probes);
   m_pResults = new ExperimentationResults(options.getNbRepetition(),m_probes);
   m_pProbeDataCollector = new ProbeDataCollector(&m_probes);
}

Experimentation::~Experimentation()
{
    delete m_pProbeDataCollector;
    delete m_pResults;
    delete m_pOverheadResults;

    for ( ProbeList::const_iterator itProbe = m_probes.begin() ; itProbe != m_probes.end() ; ++itProbe )
    {
       delete *itProbe;
    }

    m_probes.clear();
}

void Experimentation::saveResults()
{
    ExperimentationResultsWriter writer(m_options.getOutputFile());
    writer.write(*m_pOverheadResults,*m_pResults,m_probes);
}
