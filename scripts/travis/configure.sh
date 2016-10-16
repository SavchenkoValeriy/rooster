#!/bin/bash

# CMake configuration
mkdir -p build
cd build

ARGS="-DCMAKE_BUILD_TYPE=Release"

cmake .. $ARGS
