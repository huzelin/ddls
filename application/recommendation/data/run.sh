#/bin/sh
# NOTE: You should unzip the sample
sh preprocess.sh

mkdir -p samples
python gen_dist_sample.py sort_common_features_train.csv sort_sample_skeleton_train.csv 
mv samples trains

mkdir -p samples
python gen_dist_sample.py sort_common_features_test.csv sort_sample_skeleton_test.csv 
mv samples tests
