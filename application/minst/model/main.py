from models import fc_net, conv_net, AlexNet
import torch
import logging
from torch.autograd import Variable
import numpy as np
from ddls.hpps.zoo import *
from ddls.topi.torch_nn import TorchParamManager
from ddls.feeder.plan_maker import PlanMaker
from ddls.hpps.tensor import Tensor
from ddls.feeder.feeder import Feeder
from ddls.utils.log import init_logging
from ddls.utils.eval import EvalPloter

batch_size = 100
epoch = 80

def eval(model, test_iterator, eval_ploter, total_iteration):
    sum = 0.0
    accurate = 0.0
    loss_sum = 0.0
    loss_num = 0
    
    criterion = torch.nn.CrossEntropyLoss()
    
    for iteration in xrange(10000 / batch_size):
        batch = test_iterator.next_batch()
        image, label = batch.get_tensor('image').asnumpy(), batch.get_tensor('label').asnumpy().reshape([-1]).astype(np.long)
        prediction = model.forward(torch.from_numpy(image))

        loss = criterion(prediction, torch.from_numpy(label))
        loss_sum += loss.data.numpy()
        loss_num += 1
        if iteration % (1000 / batch_size) == 0:
            logging.info('test iteration=%d test_loss=%f' %(iteration, loss.data.numpy()))

        plabel = torch.max(prediction.data, 1)[1].numpy()
        sum += plabel.shape[0]
        for x in xrange(plabel.size):
            if plabel[0] == label[0]:
                accurate += 1

    logging.info('test accuracy: %f' % (accurate / sum))
    eval_ploter.add_test_plot(iter = total_iteration, loss = (loss_sum / loss_num), accuracy = (accurate / sum))

def train(model, param_manager, train_iterator, test_iterator, eval_plotter):
    # training iteration
    criterion = torch.nn.CrossEntropyLoss()
    delay = 1.0
    total_iteration = 0

    for _epoch in range(epoch):
        for iteration in xrange(60000 / batch_size):
            batch = train_iterator.next_batch()
            image, label = batch.get_tensor('image').asnumpy(), batch.get_tensor('label').asnumpy().reshape([-1])

            image_input = torch.from_numpy(image)
            image_input.requires_grad = False
            prediction = model.forward(image_input)
            #print(label.astype(np.long)) 
            label = Variable(torch.from_numpy(label.astype(np.long)), requires_grad=False)
        
            total_iteration += 1
            loss = criterion(prediction, label) 
            if iteration % (1000 / batch_size) == 0:
                logging.info('epoch=%d train iteration=%d train_loss=%f' % (_epoch, iteration, loss.data.numpy()))
                eval_ploter.add_train_plot(total_iteration, loss.data.numpy())

            loss = loss * delay
            param_manager.zero_grad()
            loss.backward()
            if image_input.requires_grad:
                print(image_input.grad.data.numpy())
            ''' 
            for name, value in model.named_parameters():
                if name == 'fc4.bias':
                    print(name)
                    print(value.data)
                    print(value.grad.data)
            '''
            param_manager.sync_all_param(option={ 'lr' : '0.01' })

            if iteration > 0 and iteration % (10000 / batch_size) == 0:
                eval(model, test_iterator, eval_ploter, total_iteration)
        #delay *= 0.9

def init_sample():
    plan_maker = PlanMaker()
    plan_maker.set_uri(['/tmp/minst_train'])
    plan_maker.set_batch_size(batch_size)
    plan_maker.set_epoch(epoch)
    train_plan = plan_maker.make()
    
    plan_maker = PlanMaker()
    plan_maker.set_uri(['/tmp/minst_t10k'])
    plan_maker.set_batch_size(batch_size)
    plan_maker.set_epoch(epoch * 6)
    test_plan = plan_maker.make()
    
    feeder = Feeder()
    
    train_iterator = feeder.schedule(train_plan, max_queue_size = 1)
    test_iterator = feeder.schedule(test_plan, max_queue_size = 1)
    
    feeder.start(thread_num = 1)

    return feeder, train_iterator, test_iterator

if __name__ == "__main__":
    #zoo_set_log_level(0)
    init_logging()
    # Start zoo
    zoo_start()
    
    # Network choise
    device = torch.device('cpu')
    model = fc_net().to(device)
    
    # Network param manager
    param_manager = TorchParamManager(model, solver='sgd')
    
    feeder, train_iterator, test_iterator = init_sample()
    eval_ploter = EvalPloter()
    
    train(model, param_manager, train_iterator, test_iterator, eval_ploter)
    eval_ploter.draw('plot.png')

    param_manager.save_model('minst.onnx',
                             torch.randn(0, 784, device='cpu'),
                             inames=['input'],
                             onames=['output'])
    
    # stop feeder
    feeder.stop()
    # Stop zoo
    zoo_stop()


