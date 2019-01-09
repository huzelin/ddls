from ddls.feeder.plan_maker import PlanMaker
from ddls.feeder.record_io import RecordIO, BinaryWrite
from ddls.feeder.feeder import Feeder
from ddls.hpps.tensor import Tensor

################################################################

##################################################################

# create plan
plan_maker = PlanMaker()
plan_maker.set_uri(
    [
     #'/tmp/sample1',
     '/tmp/minst_train',
     '/tmp/minst_t10k',
    ])
plan_maker.set_batch_size(2)
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
    print batch.get_tensor('image').asnumpy()
    print batch.get_tensor('label').asnumpy()

feeder.stop()
#print batch.get_tensor('ad').shape
#print batch.get_tensor('ad').asnumpy()
