from ddls.feeder.plan_maker import PlanMaker
from ddls.feeder.record_io import RecordIO, BinaryWrite
from ddls.feeder.feeder import Feeder
from ddls.hpps.tensor import Tensor
from ddls.feeder.utils import Id2UniqId
import numpy as np
import time

################################################################

##################################################################

# create plan
plan_maker = PlanMaker()
plan_maker.set_uri(
    [
     #'/tmp/sample1',
     './ads_data/samples/sample_4',
    ])
plan_maker.set_batch_size(128)
plan_maker.set_epoch(1)

loop = 32500
plans = []
for x in xrange(0, 1):
    plans.append(plan_maker.make())

# feeder
feeder = Feeder()

batch_iterator = []
for plan in plans:
    batch_iterator.append(feeder.schedule(plan, max_queue_size = 1))

feeder.start(thread_num=1)

start = time.time()

for x in xrange(0, loop):
    indice = x % len(plans)
    batch = batch_iterator[indice].next_batch()

    for name in batch.names():
        if name.endswith('.ids'):
            batch.add_uniqid_tensor([ name ])
        if name.endswith(".indices"):
            batch.add_indices_tensor([ name ])
      
    if x % 1000 == 0:
        print x, loop
        
        for name in batch.names():
            if name.endswith('.ids.localid'):
                feature = name.split('.')[0]
                print batch.get_tensor(name).asnumpy()
                print batch.get_tensor(feature + ".ids.uniqid").asnumpy()
                print batch.get_tensor(feature + ".values").asnumpy()

        print batch.get_tensor('label_1').asnumpy()

    #print local_id.asnumpy(), uniq_id.asnumpy()
    #print np.take(uniq_id.asnumpy(), local_id.asnumpy())
    #print batch.get_tensor('101.ids').asnumpy()
    #print batch.get_tensor('101.values').asnumpy()
    #print batch.get_tensor('109_14.ids').asnumpy(), batch.get_tensor('109_14.values').asnumpy()
    #print batch.get_tensor('110_14.ids').asnumpy(), batch.get_tensor('110_14.values').asnumpy()
    #print batch.get_tensor('205.ids').asnumpy(), batch.get_tensor('205.values').asnumpy()
    #print batch.get_tensor('label_1').asnumpy()
    #print batch.get_tensor('label_2').asnumpy()

end = time.time()
print (end - start) / loop

feeder.stop()
