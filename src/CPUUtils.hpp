#ifndef CPUUTILS_HPP
#define CPUUTILS_HPP

/**
 * Utility class to control CPU behavior
 * Thanks to this class you can pin your current process to a specific
 * core using \a pinCPU() and set the FIFO priority with \a setFifoMaxPriority()
 */
class CPUUtils
{
private:

public:
   /**
    * Pin the current process to a specific CPU core
    * \param cpuID the id or the core
    */
   static void pinCPU(int cpuID);
   
   /**
    * Sets a FIFO priority for the current process
    * If the \a offset argument is 0, the maximal priority is set. The priority
    * will be set to MAX + offset. So offset should be < 0.
    * \param offset an offset to not set the max priority
    */
   static void setFifoMaxPriority(int offset);
};

#endif
