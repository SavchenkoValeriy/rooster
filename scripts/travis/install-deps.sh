#!/bin/bash

#sudo add-apt-repository -y ppa:george-edison55/cmake-3.x
#sudo apt-get -y update
#sudo apt-get install -y --no-install-recommends \
#     build-essential \
#     clang-3.8 \
#     g++-5 \
#     libboost-dev

if [ ! -d "$DEP_DIR" ]; then
    mkdir -p "$DEP_DIR"
fi

#install clang 3.9.0
CLANG_VER="3.9.0"
CLANG_DIR="$DEP_DIR/clang-$CLANG_VER"
pushd .
echo "Looking for clang at $CLANG_DIR"
if [ ! -d "$CLANG_DIR" ]; then
    CLANG_TAR="clang+llvm-$CLANG_VER-x86_64-linux-gnu-ubuntu-16.04.tar.xz"
    mkdir $CLANG_DIR
    cd $CLANG_DIR
    wget "http://llvm.org/releases/$CLANG_VER/$CLANG_TAR"
    tar xf $CLANG_TAR -C . --strip-components=1
fi
popd

#export PATH=$CLANG_DIR/bin:$PATH
CLANG_BIN="$CLANG_DIR/bin/clang"
CLANGXX_BIN="$CLANG_BIN++"

#install hana if it's not installed
HANA_DIR="$DEP_DIR/hana"
HANA_BUILD_DIR="$HANA_DIR/build"
pushd .
echo "Looking for hana at $HANA_DIR"
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
GTEST_DIR="$DEP_DIR/gtest"
GTEST_BUILD_DIR="$GTEST_DIR/build"
echo "Looking for gtest at $GTEST_DIR"
pushd .
if [ -d "$GTEST_DIR" ]; then
    echo "Using cached gtest build ..."
    cd $GTEST_DIR/build
else
    echo "Building gtest from source ..."
    rm -rf $GTEST_DIR
    cd "$DEP_DIR"
    git clone https://github.com/google/googletest.git
    cd googletest
    git checkout -b release-1.8.0
    mkdir build
    cd build
    cmake ..
fi
sudo make install
popd


echo $PATH
ls -la /usr/bin/*++*
ls -la /usr/local/

#install json if it's not installed
JSON_DIR="$DEP_DIR/json"
JSON_BUILD_DIR="$JSON_DIR/build"
echo "Looking for json at $JSON_DIR"
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
