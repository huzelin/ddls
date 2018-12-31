import os
import struct
import numpy as np
from hpps.feeder.record_io import RecordIO, BinaryWrite
from hpps.feeder.tensor import Tensor
from hpps.feeder.feeder import Feeder

def load_mnist(path, kind='train'):
    """Load MNIST data from `path`"""
    labels_path = os.path.join(path,
                               '%s-labels-idx1-ubyte'
                               % kind)
    images_path = os.path.join(path,
                               '%s-images-idx3-ubyte'
                               % kind)
    with open(labels_path, 'rb') as lbpath:
        magic, n = struct.unpack('>II',
                                 lbpath.read(8))
        labels = np.fromfile(lbpath,
                             dtype=np.uint8)

    with open(images_path, 'rb') as imgpath:
        magic, num, rows, cols = struct.unpack('>IIII',
                                               imgpath.read(16))
        images = np.fromfile(imgpath,
                             dtype=np.uint8).reshape(len(labels), 784)

    return images, labels

def gen_sample(type):
    images, labels = load_mnist('./', type)
    print(labels.shape)
    print(images.shape)
    
    record_io = RecordIO('/tmp/minst_%s' % type, BinaryWrite)
    record_io.write_header({ 'label' : np.float32, 'image' : np.float32 })
    
    for index in xrange(labels.shape[0]):
        label = labels[index : index + 1]
        image = images[index : index + 1]

        label_tensor = Tensor([10], np.float32)
        image_tensor = Tensor([images.shape[1]], np.float32)

        r_label = np.zeros([10], np.float32)
        r_label[label[0]] = 1.0

        label_tensor.load_numpy(r_label)
        image_tensor.load_numpy(np.array(image[0], np.float32))

        record_io.write_sample({ 'label' : label_tensor, 'image' : image_tensor })
        
    record_io.write_finalize()

gen_sample('train')
gen_sample('t10k')
