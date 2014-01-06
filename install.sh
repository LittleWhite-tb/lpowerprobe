#!/bin/bash

mkdir -p build
./configure --prefix=`pwd`/build/default
make
make install
