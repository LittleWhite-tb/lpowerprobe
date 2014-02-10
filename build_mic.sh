#!/bin/sh

mkdir ./build/mic
make clean all CC=icc CXX=icpc MPICC=mpiicc LD=icpc CFLAGS=-mmic CXXFLAGS=-mmic LDFLAGS=-mmic PREFIX=`pwd`/build/mic
