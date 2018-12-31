#!/bin/sh
set -x

PWD=`dirname $0`

# install ompi
cd ${PWD}/ompi && make install
cd ../
# install zeromq
cd ${PWD}/zeromq && make install

