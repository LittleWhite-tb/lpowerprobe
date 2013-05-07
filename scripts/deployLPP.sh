#!/bin/bash

# Script to deploy lPowerProbe and give enough permissions to read MSR from userspace

NB_CORES=`grep -c ^processor /proc/cpuinfo`

cd /tmp/
git clone ~/nfs_public/work/sow2-1/gits/lPowerProbe.git/

cd lPowerProbe
make clean all

echo "From here, the script needs admin access to install lPowerProbe"
sudo make install
sudo modprobe msr

for core in `seq 0 $NB_CORES`
do
    sudo chmod -R 777 /dev/cpu/$core/*
    sudo chmod -R 777 /sys/devices/system/cpu/cpu$core/cpufreq/*
done
