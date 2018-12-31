from models import fc_net, conv_net, AlexNet
import torch
from hpps.zoo import *
from hpps.topi.torch_nn import TorchParamManager
from hpps.feeder.plan_maker import PlanMaker
from hpps.feeder.tensor import Tensor
from hpps.feeder.feeder import Feeder

# Start zoo
zoo_start()

# Network choise
device = torch.device('cpu')
model = fc_net().to(device)

# Network param manager
param_manager = TorchParamManager(model)

# Feeder
batch_size = 32
epoch = 200

plan_maker = PlanMaker()
plan_maker.set_uri(['/tmp/minst_train'])
plan_maker.set_batch_size(batch_size)
plan_maker.set_epoch(epoch)
train_plan = plan_maker.make()

plan_maker = PlanMaker()
plan_maker.set_uri(['/tmp/minst_t10k'])
plan_maker.set_batch_size(batch_size)
plan_maker.set_epoch(epoch)
test_plan = plan_maker.make()

feeder = Feeder()

train_iterator = feeder.schedule(train_plan, max_queue_size = 1)
test_iterator = feeder.schedule(test_plan, max_queue_size = 1)

feeder.start(thread_num = 1)

# training iteration
criterion = torch.nn.MSELoss()
#criterion = torch.nn.CrossEntropyLoss()

for _epoch in range(epoch):
    for iteration in xrange(60000 / batch_size):
        batch = train_iterator.next_batch()
        image, label = batch.get_tensor('image').asnumpy(), batch.get_tensor('label').asnumpy()
        prediction = model.forward(torch.from_numpy(image))
        #print(label)
        #print(prediction.data.numpy())
        
        loss = criterion(prediction, torch.from_numpy(label)) / batch_size
        print('epoch=%d iteration=%d loss=%f' % (_epoch, iteration, loss.data.numpy()))
        loss.backward()
        #for name, value in model.named_parameters():
        #    if name == 'fc4.bias':
        #        print(name)
        #        print(value.data)
        #        print(value.grad.data)
        param_manager.sync_all_param()
        #if iteration >= 10:
        #  break

# stop feeder
feeder.stop()

# Stop zoo
zoo_stop()



