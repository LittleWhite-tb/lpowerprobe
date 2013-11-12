#!/bin/bash

libtoolize
autoreconf
automake --add-missing -c
