#include "Probe.hpp"
#include "ProbeV1.hpp"

#include <iostream>
#include <dlfcn.h>

#include "ProbeInitialisationException.hpp"

ProbeV1::ProbeV1(const std::string& path)
    :Probe(path, "evaluationInit", "evaluationClose")
{
    this->evaluationStart = loadSymbol<libGet>("evaluationStart", MANDATORY);
    this->evaluationStop = loadSymbol<libGet>("evaluationStop", MANDATORY);
   
    // Now, we just init the probe
    this->init ();
}

void ProbeV1::startMeasure()
{
   m_startValue = this->evaluationStart(this->pProbeHandle);
}

double* ProbeV1::stopMeasure()
{
    m_measureValue = this->evaluationStop(this->pProbeHandle)-m_startValue;
    return &m_measureValue;
}
