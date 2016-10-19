#!/bin/bash

sudo add-apt-repository -y ppa:george-edison55/cmake-3.x
sudo apt-get -y update
sudo apt-get install -y --no-install-recommends \
     build-essential \
     libboost-dev \
     gcc \
     g++ \
     python \
     libtool \
     m4 \
     autoconf \
     automake \
     libtool \
     zlib1g-dev \
     ninja-build \
     cmake

if [ ! -d "$DEP_DIR" ]; then
    mkdir -p "$DEP_DIR"
fi

#install clang 3.9.0
CLANG_VER="39"
CLANG_DIR="$DEP_DIR/clang-$CLANG_VER"
CLANG_SOURCE="$DEP_DIR/clang-source"
pushd .
if [ ! -d "$CLANG_DIR" ]; then
    if [ ! -d "$CLANG_SOURCE" ]; then
        echo "Building LLVM/clang from source..."
        mkdir -p "$CLANG_SOURCE"
        cd "$CLANG_SOURCE"
        if [ ! -e llvm ]; then
            git clone http://llvm.org/git/llvm.git
            git checkout -b release_$CLANG_VER
        fi
        pushd .
        cd llvm/tools
        if [ ! -e clang ]; then
            git clone http://llvm.org/git/clang.git
            git checkout -b release_$CLANG_VER
        fi
        popd
        cd llvm
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$CLANG_DIR" \
              -DLLVM_ENABLE_EH=ON -DLLVM_ENABLE_RTTI=ON
        make -j5
    else
        cd "$CLANG_SOURCE"/build
    fi
    make install
fi
popd

#install hana if it's not installed
HANA_DIR="$DEP_DIR/hana"
HANA_BUILD_DIR="$HANA_DIR/build"
pushd .
if [ -d "$HANA_DIR" ]; then
    echo "Using cached hana build ..."
    cd $HANA_DIR/build
else
    echo "Building hana from source ..."
    rm -rf $HANA_DIR
    cd "$DEP_DIR"
    git clone https://github.com/boostorg/hana.git
    cd hana
    mkdir build
    cd build
    cmake ..
fi
sudo make install
popd

#install json if it's not installed
JSON_DIR="$DEP_DIR/json"
JSON_BUILD_DIR="$JSON_DIR/build"
pushd .
if [ -d "$JSON_DIR" ]; then
    echo "Using cached json build ..."
    cd $JSON_DIR/build
else
    echo "Building json from source ..."
    rm -rf $JSON_DIR
    cd "$DEP_DIR"
    git clone https://github.com/nlohmann/json.git
    cd json
    git checkout -b v2.0.0
    mkdir build
    cd build
    cmake ..
fi
sudo make install
popd
