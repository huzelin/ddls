from __future__ import print_function
import argparse
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
import numpy as np
from hpps.topi.torch_nn import TorchParamManager
from hpps.zoo import *

class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.conv1 = nn.Conv2d(1, 20, 5, 1)
        self.conv2 = nn.Conv2d(20, 50, 5, 1)
        self.fc1 = nn.Linear(4*4*50, 500)
        self.fc2 = nn.Linear(500, 10)

    def forward(self, x):
        x = F.relu(self.conv1(x))
        x = F.max_pool2d(x, 2, 2)
        x = F.relu(self.conv2(x))
        x = F.max_pool2d(x, 2, 2)
        x = x.view(-1, 4*4*50)
        x = F.relu(self.fc1(x))
        x = self.fc2(x)
        return F.log_softmax(x, dim=1)

def get_model():
    device = torch.device("cpu")
    model = Net().to(device)
    model.train()
    return model

def train(model):
    data = torch.rand(128, 1, 20, 20)
    #target = torch.empty(5, 1, 10)
    output = model(data)
    output.mean().backward()

zoo_start()

model = get_model()

param_manager = TorchParamManager(model, { 'algo' : 'assign' })

for iter in xrange(1000):
    train(model)
    for key, value in model.named_parameters():
        if key == "fc2.bias":
            print(key)
            print(value.data)
            print(value.grad.data)
    param_manager.sync_all_param()
    print('iter-%d', iter)

zoo_stop()
