#!/bin/bash

# load configuration
if [ $# -lt 1 ]; then
   echo "No configuration file provided to LPP preloader"
   exit 1
elif ! [ -f "$1" ]; then
   echo "Invalid configuration file provided to LPP preloader"
   exit 1
fi

source $1
shift

# hijack MPI calls
if [ -z "${LD_PRELOAD}" ]; then
   export LD_PRELOAD="${LPPMPILIB}"
else
   export LD_PRELOAD="${LPPMPILIB} ${LD_PRELOAD}"
fi

"$@"

