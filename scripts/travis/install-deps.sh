#!/bin/bash

sudo apt-get -y update
sudo apt-get install -y --no-install-recommends \
     build-essential

#install hana if it's not installed
HANA_DIR="${DEP_DIR}/hana"
HANA_BUILD_DIR="${HANA_DIR}/build"
pushd .
if [ -d "$HANA_DIR" ]; then
    echo "Using cached hana build ..."
    cd $HANA_DIR
else
    echo "Building hana from source ..."
    rm -rf $HANA_DIR
    cd "${DEP_DIR}"
    git clone https://github.com/boostorg/hana.git
    cd hana
    mkdir build
    cd build
    cmake ..
fi
sudo make install
popd

#install json if it's not installed
JSON_DIR="${DEP_DIR}/json"
JSON_BUILD_DIR="${JSON_DIR}/build"
pushd .
if [ -d "$JSON_DIR" ]; then
    echo "Using cached json build ..."
    cd $JSON_DIR
else
    echo "Building json from source ..."
    rm -rf $JSON_DIR
    cd "${DEP_DIR}"
    git clone https://github.com/nlohmann/json.git
    cd json
    git checkout -b v2.0.0
    mkdir build
    cd build
    cmake ..
fi
sudo make install
popd

#install clang 3.9.0
CLANG_VER="3.9.0"
CLANG_DIR="${DEP_DIR}/clang-${CLANG_VER}"
pushd .
if [ -d "$CLANG_DIR" ]; then
    CLANG_TAR="clang+llvm-${CLANG_VER}-x86_64-linux-gnu-ubuntu-16.04.tar.xz"
    mkdir $CLANG_DIR
    cd $CLANG_DIR
    wget "http://llvm.org/releases/${CLANG_VER}/${CLANG_TAR}"
    tar xf $CLANG_TAR -C . --strip-components=1
fi
popd
