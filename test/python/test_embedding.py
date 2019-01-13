from ddls.hpps.zoo import *
from ddls.hpps.array_table import ArrayTable, create_array_table
from ddls.hpps.kv_table import KVTable, create_kv_table
from ddls.hpps.tensor import Tensor
from ddls.topi.embedding import Embedding
import numpy as np

zoo_start()

embedding=Embedding(capacity=200000, value_len=24)

key = Tensor([20000], np.int64)
key_numpy = np.ones([20000], np.int64)
for x in xrange(20000):
    key_numpy[x] = x + 1
key.load_numpy(key_numpy)

grad=np.ones([20000], np.float32)

for x in xrange(100):
    key.load_numpy(np.ones([1000], np.int64))
    value = embedding.get(key, key)
    print value
    
    embedding.add(key, grad)

zoo_stop()
