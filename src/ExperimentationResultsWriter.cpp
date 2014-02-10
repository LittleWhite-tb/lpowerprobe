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

#include "ExperimentationResultsWriter.hpp"

ExperimentationResultsWriter::ExperimentationResultsWriter(const std::string& filePath)
    :m_outputFile(filePath.c_str())
{
}

void ExperimentationResultsWriter::writeHeader(std::ostream& output, const ProbeList& probes)
{
    // Special formatting for output file
    output << std::fixed;

    // Write header for file
    for ( ProbeList::const_iterator itProbe = probes.begin() ; itProbe != probes.end() ; ++itProbe)
    {
        output << (*itProbe)->getLabel();
        for ( unsigned int i = 0 ; i < (*itProbe)->getNbDevices() * (*itProbe)->getNbChannels() ; i++ )
        {
            output << ";";
        }
    }
    output << std::endl;
}

void ExperimentationResultsWriter::write(const ExperimentationResults& overheaderResults, const ExperimentationResults& results, const ProbeList& probes)
{
   std::ostream* output = &m_outputFile;
   if ( !m_outputFile.is_open())
   {
      output = &std::cout;
      std::cerr << "The results will be outputted on console since we can't write in the file" << std::endl;
   }

   // We save only the results for the first process

   // compute the average overhead
   std::vector<ProbeData*> libsOverheadAvg(probes.size(),NULL);
   unsigned int r = 0;
   for ( std::vector<RunData>::const_iterator itMRepet = overheaderResults.getResults().begin() ;
         r < overheaderResults.getNbResults() && itMRepet != overheaderResults.getResults().end() ; ++itMRepet, ++r )
   {
      for ( unsigned int i = 0 ; i < itMRepet->getData().size () ; i++ )
      {
         const ProbeData& pd = itMRepet->getProbeData(i);
         if ( libsOverheadAvg[i] == NULL )
         {
            libsOverheadAvg[i] = new ProbeData(pd.getNbDevices(),pd.getNbChannels());
         }

         // We only manage the first (single shot) libraries
         (*libsOverheadAvg[i]) += pd;
      }
   }

   for ( std::vector<ProbeData*>::iterator itData = libsOverheadAvg.begin() ; itData != libsOverheadAvg.end() ; ++itData )
   {
      (*(*itData)) /= overheaderResults.getNbResults();
   }

   writeHeader(*output,probes);

   for ( unsigned int mRepet = 0 ; mRepet < results.getNbResults() ; mRepet++ )
   {
      for ( unsigned int i = 0 ; i < results.getResults()[mRepet].getNbProbeData() ; i++ )
      {
         const ProbeData& rawProbeData = results.getResults()[mRepet].getProbeData(i);

         ProbeData* pPD = rawProbeData -(*libsOverheadAvg[i]);

         (*output) << *pPD;
         if ( i ==  results.getResults()[mRepet].getNbProbeData() -1)
         {
            (*output) << std::endl;
         }

         delete pPD;
      }
   }

   for ( std::vector<ProbeData*>::iterator itData = libsOverheadAvg.begin() ; itData != libsOverheadAvg.end() ; ++itData )
   {
      delete *itData;
   }

   output->flush();
}
