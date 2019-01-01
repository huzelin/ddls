# DDLS

Travis CI: [![Build Status](https://travis-ci.org/huzelin/ddls.svg?branch=master)](https://travis-ci.org/huzelin/ddls)

## Introduction
**DDLS** is a parameter server based Distributed Deep Learning Studio for training machine learning models on Big Data with a numbers of machines, which also contains a few algorithm solution on Urban Computing, such as: Traffic Forecasting etc.

## Installation from source

Download from project homepage. https://github.com/huzelin/ddls.git

### Prerequisites

   * gcc >= 4.8.5
   * cmake >= 2.8.2

### Install to a specific directory

Use **-DCMAKE\_INSTALL\_PREFIX=**, for example

```bash
  mkdir build && cd build && cmake ../ -DCMAKE_INSTALL_PREFIX=/opt/ddls/ && make
  sudo make install
  cd ../binding/python && sudo python setup.py install
```

The default installation directory is /opt/ddls/

## Applications

  * See [minst](application/minst) for details

  * See [recommendation](application/recommendation) for details

  * See [urban computing](application/urban_computing) for details

## Benchmark

  * TODO Later

