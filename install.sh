#!/bin/bash

mkdir -p build
./configure --prefix=`pwd`/build
make
make install
