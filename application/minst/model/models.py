from __future__ import print_function
import argparse
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
import numpy as np

def to_image(data):
    data = data.view(-1, 1, 28, 28)
    return data

class fc_net(nn.Module):
    """ fully connected network
    """
    def __init__(self):
        super(fc_net, self).__init__()
        self.fc1 = torch.nn.Sequential(torch.nn.Linear(784, 200), torch.nn.ReLU())
        self.fc2 = torch.nn.Sequential(torch.nn.Linear(200, 100), torch.nn.ReLU())
        self.fc3 = torch.nn.Sequential(torch.nn.Linear(100, 20), torch.nn.ReLU())
        self.fc4 = torch.nn.Linear(20, 10)
        self.softmax = torch.nn.Softmax(dim=1)

    def forward(self, x):
        x = self.fc1(x)
        x = self.fc2(x)
        x = self.fc3(x)
        x = self.fc4(x)
        x = self.softmax(x)
        return x

class conv_net(nn.Module):
    """ conv network
    """
    def __init__(self):
        super(conv_net, self).__init__()
        self.conv1 = torch.nn.Sequential(torch.nn.Conv2d(1, 10, 5, 1, 1),
                                         torch.nn.MaxPool2d(2),
                                         torch.nn.ReLU(),
                                         torch.nn.BatchNorm2d(10))
        self.conv2 = torch.nn.Sequential(torch.nn.Conv2d(10, 20, 5, 1, 1),
                                         torch.nn.MaxPool2d(2),
                                         torch.nn.ReLU(),
                                         torch.nn.BatchNorm2d(20))
        self.fc1 = torch.nn.Sequential(torch.nn.Linear(500, 60),
                                       torch.nn.Dropout(0.5),
                                       torch.nn.ReLU())
        self.fc2 = torch.nn.Sequential(torch.nn.Linear(60, 20),
                                       torch.nn.Dropout(0.5),
                                       torch.nn.ReLU())
        self.fc3 = torch.nn.Linear(20, 10)

    def forward(self, x):
        x = self.conv1(x)
        x = self.conv2(x)
        x = x.view(-1, 500)
        x = self.fc1(x)
        x = self.fc2(x)
        x = self.fc3(x)
        return x

class AlexNet(nn.Module):
    """ like AlexNet
    """
    def __init__(self, num_classes = 10):
        super(AlexNet, self).__init__()
        self.features = torch.nn.Sequential(torch.nn.Conv2d(1, 64, kernel_size=5, stride=1, padding=2),
                                            torch.nn.ReLU(inplace=True),
                                            torch.nn.MaxPool2d(kernel_size=3, stride=1),
                                            torch.nn.Conv2d(64, 192, kernel_size=3, padding=2),
                                            torch.nn.ReLU(inplace=True),
                                            torch.nn.MaxPool2d(kernel_size=3, stride=2),
                                            torch.nn.Conv2d(192, 384, kernel_size=3, padding=1),
                                            torch.nn.ReLU(inplace=True),
                                            torch.nn.Conv2d(384, 256, kernel_size=3, padding=1),
                                            torch.nn.ReLU(inplace=True),
                                            torch.nn.Conv2d(256, 256, kernel_size=3, padding=1),
                                            torch.nn.ReLU(inplace=True),
                                            torch.nn.MaxPool2d(kernel_size=3, stride=2))
        self.classifier = torch.nn.Sequential(torch.nn.Dropout(),
                                              torch.nn.Linear(256 * 6 * 6, 4096),
                                              torch.nn.ReLU(inplace=True),
                                              torch.nn.Dropout(),
                                              torch.nn.Linear(4096, 4096),
                                              torch.ReLU(inplace=True),
                                              torch.nn.Linear(4096, num_classes))

    def forward(self, x):
        x = self.features(x)
        x = x.view(x.size(0), 256 * 6 * 6)
        x = self.classifier(x)
        return x
