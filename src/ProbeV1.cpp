#include "ProbeV1.hpp"

#include <iostream>
#include <dlfcn.h>

#include "ProbeInitialisationException.hpp"

ProbeV1::ProbeV1(const std::string& path)
    :Probe(path)
{
    this->evaluationInit = loadSymbol<libInit>("evaluationInit");
    this->evaluationFini = loadSymbol<libFini>("evaluationClose");
    this->evaluationStart = loadSymbol<libGet>("evaluationStart");
    this->evaluationStop = loadSymbol<libGet>("evaluationStop");

    std::cout << "'" << path << "' successfully loaded" << std::endl;
   
    // Now, we just start the probe
    if ( this->evaluationInit )
    {
        this->pProbeHandle = this->evaluationInit();
        if ( this->pProbeHandle == NULL )
        {
            throw ProbeInitialisationException(path);
        }
    }
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
