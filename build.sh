#!/bin/bash

if [ ! -d build ]; then
    mkdir build
fi

pushd build
    cmake .. -DUSE_MPI=ON -DMPI_PATH=/usr/local/ 
    make -j32 
popd
