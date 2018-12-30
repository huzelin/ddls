from hpps.feeder.plan_maker import PlanMaker
from hpps.feeder.record_io import RecordIO, BinaryWrite
from hpps.feeder.tensor import Tensor
from hpps.feeder.feeder import Feeder

import numpy as np

################################################################

record_io = RecordIO('/tmp/sample1', BinaryWrite)
record_io.write_header({ 'ad' : np.float32, 'user' : np.float32 })

ad_tensor = Tensor([300], np.float32)
ad_tensor.load_numpy(np.random.rand(300))
user_tensor = Tensor([400], np.float32)
user_tensor.load_numpy(np.random.rand(400))

for x in xrange(1, 1000000):
    record_io.write_sample({ 'ad' : ad_tensor, 'user' : user_tensor })
record_io.write_finalize()

##################################################################
