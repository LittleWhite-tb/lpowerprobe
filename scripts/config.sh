#!/bin/bash

# configuration file for MPI lpp

# directory containing lPowerProbe
LPP_DIR="${HOME}/nfs/lPowerProbe"

# shared location where to copy final results from the local output
RES_DIR="${HOME}/nfs"

# Nb of repetitions for the program to run
NREPET=5

# list of probes to run
LPPLIBS="${LPP_DIR}/probes/libwallclock/libwallclock.so"


# --- You should probably not edit the lines below ---


# output file for LPP (warning: the location is local to each node)
LPPOUTPUT="/tmp/output.csv"

# options for lPowerProbe on each node
LPPOPTS="-l ${LPPLIBS} -o ${LPPOUTPUT}"

# paths
LPP="${LPP_DIR}/lPowerProbe"
LPPLOADER="${LPP_DIR}/scripts/MPI_preload/preloader.sh"
LPPKILLER="${LPP_DIR}/scripts/lppMPIkiller.sh"
LPPMPILIB="${LPP_DIR}/scripts/MPI_preload/liblppMPInotifier.so"

LPPPIDFILE="/tmp/lppDaemonPID"

