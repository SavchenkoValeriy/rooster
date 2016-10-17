#!/bin/bash

set -x

# CMake configuration
mkdir -p build
cd build

CLANG_CMAKE=-DClang_DIR=${CLANG_DIR}/lib/cmake/clang
LLVM_CMAKE=-DLLVM_DIR=${CLANG_DIR}/lib/cmake/llvm
ARGS="-DCMAKE_BUILD_TYPE=Release ${CLANG_CMAKE} ${LLVM_CMAKE}"

cmake .. $ARGS
