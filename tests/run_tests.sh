#!/bin/bash

if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

LPOWERPROBE_PATH=..
# All path should start from LPOWERPROBE_PATH (so, where lPowerProbe is)
TESTS_DIR=${PWD}
RESULTS_DIR=$TESTS_DIR/output

NB_CORES=`grep -c ^processor /proc/cpuinfo`
PINNING="0"
for pin in `seq 1 $(($NB_CORES-1))`
do
    PINNING="$PINNING;$pin"
done

sudo modprobe msr

# Moving in lPowerProbe dir
cd $LPOWERPROBE_PATH
./configure --prefix=${PWD}/tests/lp_bin
make clean all install
cd ./tests/lp_bin/bin


mkdir -p $RESULTS_DIR

for testEntries in `ls $TESTS_DIR/`
do
    # Checks if it is a dir and if it is not the results dir
    if [ -d $TESTS_DIR/$testEntries ] && [ "$TESTS_DIR/$testEntries" != "$RESULTS_DIR" ]
    then
        if [ "$(echo $testEntries | cut -d "_" -f 1)" = "kernel" ]
        then
            # Kernels
            ./lPowerProbe -r 100000 -m 8 -i 10000000 -o $RESULTS_DIR/lpowerprobe_$testEntries.res $TESTS_DIR/$testEntries/test.s
            ./lPowerProbe -r 100000 -m 8 -i 10000000 -d $NB_CORES -p "$PINNING"  -o $RESULTS_DIR/lpowerprobe_${testEntries}_${NB_CORES}.res $TESTS_DIR/$testEntries/test.s
        else
            ./lPowerProbe -r 5 -o $RESULTS_DIR/lpowerprobe_$testEntries.res $TESTS_DIR/$testEntries/test
            ./lPowerProbe -r 5 -d $NB_CORES -p "$PINNING"  -o $RESULTS_DIR/lpowerprobe_${testEntries}_${NB_CORES}.res $TESTS_DIR/$testEntries/test
        fi
    fi
done 
