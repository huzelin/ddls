# DDLS

Travis CI: [![Build Status](https://travis-ci.org/huzelin/ddls.svg?branch=master)](https://travis-ci.org/huzelin/ddls)

## Introduction

***DDLS*** is a parameter server(Refactored from [Multiverso](https://github.com/Microsoft/Multiverso)) based Distributed Deep Learning Studio for training deep learning models on Big Data with a numbers of machines and deploying high-performance online model service.

## Installation from source

Download from project homepage. https://github.com/huzelin/ddls.git

```bash
  git clone https://github.com/huzelin/ddls.git --recursive
```

### Prerequisites

   * gcc >= 4.8.5
   * cmake >= 2.8

### Install to a specific directory

Use **-DCMAKE\_INSTALL\_PREFIX=**, for example

```bash
  mkdir build && cd build && cmake ../ -DCMAKE_INSTALL_PREFIX=/opt/ddls/ && make
  sudo make install
  cd ../binding/python && sudo python setup.py install
```

The default installation directory is /opt/ddls/

## Applications

  * [recommendation](application/recommendation)
  * [urban computing](application/urban_computing)


