from ddls.hpps.zoo import *
from ddls.hpps.array_table import ArrayTable, create_array_table
from ddls.hpps.kv_table import KVTable, create_kv_table
from ddls.hpps.tensor import Tensor
import numpy as np

zoo_start()

array_table = create_array_table(size=100, type=np.float32, solver="default")
kv_table = create_kv_table(capacity=1000000, value_len=2, key_type=np.int64, value_type=np.float32)
zoo_barrier()

data = Tensor(shape = [100], type= np.float32)
array_table.get(value=data)
print data.asnumpy()

grad = Tensor(shape = [100], type = np.float32)
grad.load_numpy(np.ones([100], np.float32))
array_table.add(grad=grad)

array_table.get(value=data)
print data.asnumpy()

id = Tensor(shape = [100], type=np.int64)
id.load_numpy(np.zeros([100], np.int64))
value = Tensor(shape = [1], type=np.float32)
kv_table.get(key=id, value=value)
print value.asnumpy()

id.load_numpy(np.ones([100], np.int64))
wait_value = Tensor(shape=[1], type=np.float32)
wait_id = kv_table.get_async(key=id, value=wait_value)
kv_table.get_async(wait_id=wait_id, key=id, value=wait_value)
print wait_value.asnumpy()

assert isinstance(wait_value, Tensor)

kv_table.store('./xxx')
array_table.store('./yyy')

zoo_stop()
