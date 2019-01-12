import sys
from ddls.feeder.record_io import RecordIO, BinaryWrite
from ddls.hpps.tensor import Tensor
import numpy as np

feaWidth = {'216': 1, '210': 38, '702': 1, '207': 1, '853': 25, '508': 1, '509': 1, '110_14': 1001, '205': 1, '109_14': 1001, '206': 1, '301': 1, '121': 1, '122': 1, '124': 1, '125': 1, '126': 1, '127': 1, '128': 1, '129': 1, '101': 1, '150_14': 335, '127_14': 1001}

def get_feature(fd, num_index, fea_index):
    line = fd.readline()
    if line is None or len(line) == 0:
        return None
    
    segments = line.split(',')
    features = segments[fea_index].split('\001')
    assert int(segments[num_index]) == len(features)
    
    dict = { }
    for feature in features:
        feature_name, kv_list = feature.split('\002')
        kv_list_pair = kv_list.split('\003')
        assert 2 == len(kv_list_pair) 
        if dict.has_key(feature_name):
            dict[feature_name].append([(int)(kv_list_pair[0]), float(kv_list_pair[1])])
        else:
            dict[feature_name] = [[(int)(kv_list_pair[0]), float(kv_list_pair[1])]]

    label = None
    if num_index >= 3:
        label = [float(segments[num_index - 3]), float(segments[num_index - 2])]

    return segments[num_index - 1], dict, label

def get_feature_width(fd, num_index, fea_index):
    line = fd.readline()
    if line is None or len(line) == 0:
        return None
    
    segments = line.split(',')
    features = segments[fea_index].split('\001')
    assert int(segments[num_index]) == len(features)
    
    dict = { }
    for feature in features:
        feature_name, kv_list = feature.split('\002')
        kv_list_pair = kv_list.split('\003')
        assert 2 == len(kv_list_pair) 
        if dict.has_key(feature_name):
            dict[feature_name] = dict[feature_name] + 1
        else:
            dict[feature_name] = 1 

    return dict

def gen_fea_width(file, num_index, fea_index):
    fd = open(file)
    line = 0
    while True:
        dict = get_feature_width(fd, num_index, fea_index)
        if dict is None:
            break

        for key, value in dict.iteritems():
            if feaWidth.has_key(key):
                feaWidth[key] = value if value > feaWidth[key] else feaWidth[key]
            else:
                feaWidth[key] = value
        line += 1
        if line % 10000 == 0:
            print('current gen fea_width %d' % line)
            print(feaWidth)
    return line

worker_number = 10
curr_index = 0
meta = { 
        'label_1' : np.float32,
        'label_2' : np.float32,
        'indices' : np.int32,
        '101.ids' : np.int64,
        '101.values' : np.float32,
        '109_14.ids' : np.int64,
        '109_14.values' : np.float32,
        '110_14.ids' : np.int64,
        '110_14.values' : np.float32,
        '127_14.ids' : np.int64,
        '127_14.values' : np.float32,
        '150_14.ids' : np.int64,
        '150_14.values' : np.float32,
        '121.ids' : np.int64,
        '121.values' : np.float32,
        '122.ids' : np.int64,
        '122.values' : np.float32,
        '124.ids' : np.int64,
        '124.values' : np.float32,
        '125.ids' : np.int64,
        '125.values' : np.float32,
        '126.ids' : np.int64,
        '126.values' : np.float32,
        '127.ids' : np.int64,
        '127.values' : np.float32,
        '128.ids' : np.int64,
        '128.values' : np.float32,
        '129.ids' : np.int64,
        '129.values' : np.float32,
        '205.ids' : np.int64,
        '205.values' : np.float32,
        '206.ids' : np.int64,
        '206.values' : np.float32,
        '207.ids' : np.int64,
        '207.values' : np.float32,
        '210.ids' : np.int64,
        '210.values' : np.float32,
        '216.ids' : np.int64,
        '216.values' : np.float32,
        '508.ids' : np.int64,
        '508.values' : np.float32,
        '509.ids' : np.int64,
        '509.values' : np.float32,
        '702.ids' : np.int64,
        '702.values' : np.float32,
        '853.ids' : np.int64,
        '853.values' : np.float32,
        '301.ids' : np.int64,
        '301.values' : np.float32
    }

user_feature = [
    '101',
    '109_14',
    '110_14',
    '127_14',
    '150_14',
    '121',
    '122',
    '124',
    '125',
    '126',
    '127',
    '128',
    '129'
]

def write_sample(comm_feature, group_feature, group_label, record_io):
    global curr_index
 
    tensor_map = { }  
    for name, type in meta.iteritems():
        if name == 'label_1':
            value = np.zeros([len(group_label), 1], type) 
            for index in xrange(len(group_label)):
                value[index] = [group_label[index][0]]
            label_tensor = Tensor(value.shape, type)
            label_tensor.load_numpy(value)
            tensor_map[name] = label_tensor
        elif name == 'label_2':
            value = np.zeros([len(group_label), 1], type)
            for index in xrange(len(group_label)):
                value[index] = [group_label[index][1]]
            label_tensor = Tensor(value.shape, type)
            label_tensor.load_numpy(value)
            tensor_map[name] = label_tensor
        elif name == 'indices':
            value = np.zeros([1], type)
            value[0] = len(group_feature)
            indices_tensor = Tensor(value.shape, type)
            indices_tensor.load_numpy(value)
            tensor_map[name] = indices_tensor
        else:
            # real feature
            splits = name.split('.')
            pos = 0 if splits[1] == 'ids' else 1
            feature_name = splits[0]
            width = feaWidth[feature_name]
            if feature_name in user_feature:
                value = np.zeros([1, width], type)
                if comm_feature.has_key(feature_name):
                    data = comm_feature[feature_name]
                    for index in xrange(len(data)):
                        value[0][index] = data[index][pos]
            else:
                value = np.zeros([len(group_feature), width], type)
                for row in xrange(len(group_feature)):
                    if group_feature[row].has_key(feature_name):
                        data = group_feature[row][feature_name]
                        for index in xrange(len(data)):
                            value[row][index] = data[index][pos]

            feature_tensor = Tensor(value.shape, type)
            feature_tensor.load_numpy(value)
            tensor_map[name] = feature_tensor

    record_io[curr_index].write_sample(tensor_map)
    curr_index = (curr_index + 1) % worker_number

def gen_sample(comm_file, comm_num_index, comm_fea_index, skeleton_file, skeleton_num_index, skeleton_fea_index, total_line):
    record_io = []
    name = []
    type = []
    level = []
    is_aux_number = []
    for key, value in meta.iteritems():
        name.append(key)
        type.append(value)
        if key in user_feature:
            level.append(1)
        else:
            level.append(0)
        if key == "indices":
            is_aux_number.append(1)
        else:
            is_aux_number.append(0)

    for i in xrange(worker_number):
        record_io.append(RecordIO('./samples/sample_%d' % i, BinaryWrite))
        record_io[i].write_header(name, type, level, is_aux_number)

    tag = 1
    line = 0
    comm_fd = open(comm_file)
    skeleton_fd = open(skeleton_file)
    group_feature = [ ]
    group_label = []
    while True:
        common_id, comm_feature, _ = get_feature(comm_fd, comm_num_index, comm_fea_index)
        while True:
            skeleton_id, skeleton_feature, label = get_feature(skeleton_fd, skeleton_num_index, skeleton_fea_index)
            if skeleton_feature == None:
                for i in xrange(worker_number):
                    record_io[i].write_finalize()
                return
            line = line + 1
            if skeleton_id == common_id:
                group_feature.append(skeleton_feature)
                group_label.append(label)
            else:
                if line / 10000 == tag:
                    print('process %d->%d' % (line, total_line))
                    tag += 1
                write_sample(comm_feature, group_feature, group_label, record_io)
                common_id, comm_feature, _ = get_feature(comm_fd, comm_num_index, comm_fea_index)
                group_feature, group_label = [ skeleton_feature ], [ label ]
                assert common_id == skeleton_id

if __name__ == '__main__':
    line = 0
    # Step1: Read width argv[1]-> common argv[2]->skeleton
    #gen_fea_width(sys.argv[1], 1, 2)
    #line = gen_fea_width(sys.argv[2], 4, 5)
    print(feaWidth)
    # Step2: Generate Sample
    gen_sample(sys.argv[1], 1, 2, sys.argv[2], 4, 5, line)

