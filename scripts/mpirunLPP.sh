#!/bin/bash

LPP="lPowerProbe"
LPPLOADER="MPI_preload/preloader.sh"
LPPKILLER="lppMPIkiller.sh"

# run lpp as a deamon on all nodes
mpirun --tasks-per-node 1 "${LPP} &"

# actually run the program
args=""
execFound=false
for arg in "$@"; do
   if [[ ${arg} == -* ]] || ${execFound} ; then
      args=${args} "${arg}"
   else
      args=${args} "${LPPLOADER}" "${arg}"
      execFound=true
   fi
done
mpirun ${args}

# now kill the LPP daemon
mpirun --tasks-per-node 1 ${LPPKILLER}
