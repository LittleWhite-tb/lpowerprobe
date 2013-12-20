#!/bin/bash

mkdir -p build/mic
CC=icc CXX=icpc LC=icc CPPFLAGS=-mmic LDFLAGS=-mmic ./configure --host=x86_64 --prefix=`pwd`/build/mic --with-mic
make
make install
