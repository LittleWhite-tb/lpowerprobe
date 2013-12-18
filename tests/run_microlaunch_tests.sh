#!/bin/bash

if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

MICROLAUNCH_PATH=/home/users/lalexandre/Work/microlaunch/
MICROLAUNCH=$MICROLAUNCH_PATH/microlaunch
# All path should start from LPOWERPROBE_PATH (so, where lPowerProbe is)
TESTS_DIR=.
RESULTS_DIR=$TESTS_DIR/output

WALLCLOCK_LIB=${MICROLAUNCH_PATH}"/Libraries/wallclock/wallclock.so"
MSR_LIB=${MICROLAUNCH_PATH}"/Libraries/power_snb_msr/libraries/energy_msr_snb.so"

NB_CORES=`grep -c ^processor /proc/cpuinfo`
PINNING="0"
for pin in `seq 1 $(($NB_CORES-1))`
do
    PINNING="$PINNING;$pin"
done

sudo modprobe msr

mkdir -p $RESULTS_DIR

for testEntries in `ls $TESTS_DIR/`
do
    # Checks if it is a dir and if it is not the results dir
    if [ -d $TESTS_DIR/$testEntries ] && [ "$TESTS_DIR/$testEntries" != "$RESULTS_DIR" ]
    then
        if [ "$(echo $testEntries | cut -d "_" -f 1)" = "kernel" ]
        then
            # Kernels
            $MICROLAUNCH --evallib "$WALLCLOCK_LIB;$MSR_LIB" --info "raw;raw" --startvector 500000 --data-size 8 --repetition 1 --metarepetition 5 --kernelname $TESTS_DIR/$testEntries/test.s
            mv $RESULTS_DIR/kernel_default_500000.csv $RESULTS_DIR/microlaunch_kernel_$testEntries.res
            $MICROLAUNCH --evallib "$WALLCLOCK_LIB;$MSR_LIB" --info "raw;raw" --startvector 500000 --data-size 8 --repetition 1 --metarepetition 5 --kernelname $TESTS_DIR/$testEntries/test.s
            mv $RESULTS_DIR/kernel_default_500000.csv $RESULTS_DIR/microlaunch_kernel_${testEntries}_${NB_CORES}.res
        else
            $MICROLAUNCH --evallib "$WALLCLOCK_LIB;$MSR_LIB" --info "raw;raw" --repetition 1 --metarepetition 5 --execname $TESTS_DIR/$testEntries/test
            mv $RESULTS_DIR/exec_default.csv $RESULTS_DIR/microlaunch_$testEntries.res
            $MICROLAUNCH --evallib "$WALLCLOCK_LIB;$MSR_LIB" --info "raw;raw" --nbprocess $NB_CORES --repetition 1 --metarepetition 5 --execname $TESTS_DIR/$testEntries/test
            mv $RESULTS_DIR/exec_default.csv $RESULTS_DIR/microlaunch_$testEntries_${NB_CORES}.res
        fi
    fi
done 
