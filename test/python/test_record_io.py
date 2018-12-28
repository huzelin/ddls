from hpps.feeder.plan_maker import PlanMaker
from hpps.feeder.record_io import RecordIO, BinaryWrite
from hpps.feeder.tensor import Tensor, FLOAT32
from hpps.feeder.feeder import Feeder

################################################################

record_io = RecordIO('/tmp/sample1', BinaryWrite)
record_io.write_header({ 'ad' : FLOAT32, 'user' : FLOAT32 })

ad_tensor = Tensor([200, 300], FLOAT32)
user_tensor = Tensor([200, 400], FLOAT32)

record_io.write_sample({ 'ad' : ad_tensor, 'user' : user_tensor })
record_io.write_sample({ 'ad' : ad_tensor, 'user' : user_tensor })
record_io.write_sample({ 'ad' : ad_tensor, 'user' : user_tensor })
record_io.write_sample({ 'ad' : ad_tensor, 'user' : user_tensor })
record_io.write_finalize()

##################################################################
