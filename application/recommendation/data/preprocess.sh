#!/bin/sh
cat sample_skeleton_train.csv | sort -t "," -k 4 > sort_sample_skeleton_train.csv
cat common_features_train.csv | sort -t "," -k 1 > sort_common_features_train.csv

cat sample_skeleton_test.csv | sort -t "," -k 4 > sort_sample_skeleton_test.csv
cat common_features_test.csv | sort -t "," -k 1 > sort_common_features_test.csv
