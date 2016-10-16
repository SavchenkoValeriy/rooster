#!/bin/bash

apt-get -y update
apt-get install -y --no-install-recommends \
        build-essential \
        clang-3.8 \
        libboost1.58-dev
