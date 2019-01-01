# UCDL

Travis CI: [![Build Status](https://travis-ci.org/huzelin/ucdl.svg?branch=master)](https://travis-ci.org/huzelin/ucdl)

## Introduction
**UCDL** is a parameter server based framework for training machine learning models on Big Data of Urban Computing(such as: Traffic Forecasting etc.) with a numbers of machines.

## Installation from source

Download from project homepage. https://github.com/huzelin/ucdl.git

### Prerequisites

   * gcc >= 4.8.5
   * cmake >= 2.8.2

### Install to a specific directory

Use **-DCMAKE\_INSTALL\_PREFIX=**, for example

```bash
  mkdir build && cd build && cmake ../ -DCMAKE_INSTALL_PREFIX=/opt/ucdl/ && make
  sudo make install
  cd ../binding/python && sudo python setup.py install
```

The default installation directory is /opt/ucdl/

## Applications

  * See [minst](application/minst) for details

  * See [recommendation](application/recommendation) for details

  * See [urban computing](application/urban_computing) for details

## Benchmark

  * TODO Later

