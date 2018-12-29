from hpps.feeder.plan_maker import PlanMaker
from hpps.feeder.record_io import RecordIO, BinaryWrite
from hpps.feeder.tensor import Tensor
from hpps.feeder.feeder import Feeder

import numpy as np

################################################################

record_io = RecordIO('/tmp/sample1', BinaryWrite)
record_io.write_header({ 'ad' : np.float32, 'user' : np.float32 })

ad_tensor = Tensor([200, 300], np.float32)
user_tensor = Tensor([200, 400], np.float32)
np_ad = np.ones([200, 300], np.float32)
ad_tensor.load_numpy(np_ad)

record_io.write_sample({ 'ad' : ad_tensor, 'user' : user_tensor })
record_io.write_sample({ 'ad' : ad_tensor, 'user' : user_tensor })
record_io.write_sample({ 'ad' : ad_tensor, 'user' : user_tensor })
record_io.write_sample({ 'ad' : ad_tensor, 'user' : user_tensor })
record_io.write_finalize()

##################################################################
