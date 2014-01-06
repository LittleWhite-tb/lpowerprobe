#!/bin/bash

mkdir -p build/mic
CC=icc CFLAGS=-mmic CXX=icpc LC=icc CPPFLAGS=-mmic LDFLAGS=-mmic ./configure --host=x86_64-unknown-linux-gnu --prefix=`pwd`/build/mic --with-mic
make
make install
