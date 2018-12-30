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

plans = []
for x in xrange(0, 5):
    plans.append(plan_maker.make())

# feeder
feeder = Feeder()

batch_iterator = []
for plan in plans:
    batch_iterator.append(feeder.schedule(plan, max_queue_size = 1))

feeder.start(thread_num=1)

for x in xrange(1, 200):
    indice = x % len(plans)
    batch = batch_iterator[indice].next_batch()
    print batch, indice, x
    #print batch.get_tensor('ad').asnumpy()

feeder.stop()
#print batch.get_tensor('ad').shape
#print batch.get_tensor('ad').asnumpy()
