#!/bin/bash

apt-get -y update
apt-get install -y --no-install-recommends \
        build-essential \
        clang-3.8 \
        llvm-3.8-dev \
        libboost-dev

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
make install
popd

#install hana if it's not installed
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
    mkdir build
    cd build
    cmake ..
fi
make install
popd
