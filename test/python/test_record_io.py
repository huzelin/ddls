from ddls.feeder.plan_maker import PlanMaker
from ddls.feeder.record_io import RecordIO, BinaryWrite
from ddls.hpps.tensor import Tensor
from ddls.feeder.feeder import Feeder

import numpy as np

################################################################

record_io = RecordIO('/tmp/sample1', BinaryWrite)
record_io.write_header(name=['ad', 'user' ], type=[np.float32, np.float32])

ad_tensor = Tensor([300], np.float32)
ad_tensor.load_numpy(np.random.rand(300))
print(ad_tensor.asnumpy())
user_tensor = Tensor([400], np.float32)
user_tensor.load_numpy(np.random.rand(400))

for x in xrange(1, 1000):
    record_io.write_sample({ 'ad' : ad_tensor, 'user' : user_tensor })
record_io.write_finalize()

##################################################################
