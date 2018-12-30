from hpps.feeder.plan_maker import PlanMaker
from hpps.feeder.record_io import RecordIO, BinaryWrite
from hpps.feeder.tensor import Tensor
from hpps.feeder.feeder import Feeder

################################################################

##################################################################

# create plan
plan_maker = PlanMaker()
plan_maker.set_uri(
    ['/tmp/sample1',
     '/tmp/sample1',
     '/tmp/sample1',
     '/tmp/sample1',
     '/tmp/sample1',
     '/tmp/sample1',
     '/tmp/sample1',
     '/tmp/sample1',
     '/tmp/sample1'
    ])
plan_maker.set_batch_size(8192)
plan_maker.set_epoch(10)
plan = plan_maker.make()

# feeder
feeder = Feeder()
batch_iterator = feeder.schedule(plan, 1)
feeder.start(thread_num=1)

for x in xrange(1, 10000):
    batch = batch_iterator.next_batch()
    print batch, x
    #print batch.get_tensor('ad').asnumpy()
feeder.stop()
#print batch.get_tensor('ad').shape
#print batch.get_tensor('ad').asnumpy()
