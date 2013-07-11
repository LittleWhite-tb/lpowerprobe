#!/bin/bash

# can be relative (path taken from LD_LIBRARY_PATH) or absolute
MPILIB="${HOME}/nfs/lPowerProbe/scripts/MPI_preload/liblppMPInotifier.so"

if [ -z "${LD_PRELOAD}" ]; then
   export LD_PRELOAD="${MPILIB}"
else
   export LD_PRELOAD="${MPILIB} ${LD_PRELOAD}"
fi

"$@"

