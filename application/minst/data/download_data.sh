#!/bin/sh
# training set images
wget http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz
gzip -d train-images-idx3-ubyte.gz
# training set labels
wget http://yann.lecun.com/exdb/mnist/train-labels-idx1-ubyte.gz
gzip -d train-labels-idx1-ubyte.gz
# test set images
wget http://yann.lecun.com/exdb/mnist/t10k-images-idx3-ubyte.gz
gzip -d t10k-images-idx3-ubyte.gz
# test set labels
wget http://yann.lecun.com/exdb/mnist/t10k-labels-idx1-ubyte.gz
gzip -d t10k-labels-idx1-ubyte.gz
