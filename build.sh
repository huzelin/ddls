#!/bin/bash

if [ ! -d build ]; then
    mkdir build
fi

pushd build
    cmake .. -DMPI_PATH=/usr/local/ 
    make -j32 
popd
