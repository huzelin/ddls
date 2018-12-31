from __future__ import absolute_import

import os
import sys
import time
import types
import logging
import numpy as np
import matplotlib.pyplot as plt
plt.switch_backend('agg')

def calc_auc(raw_arr):
    """ Calc AUC
    """
    arr = sorted(raw_arr, key=lambda d:d[2])
    
    auc = 0.0
    fp1, tp1, fp2, tp2 = 0.0, 0.0, 0.0, 0.0
    for record in arr:
        fp2 += record[0] 
        tp2 += record[1]
        auc += (fp2 - fp1) * (tp2 - tp1)
        fp1, tp1 = fp2, tp2

    threshold = len(arr) - 1e-3
    if tp2 > threshold or fp2 > threshold:
        return -0.5

    if tp2 * fp2 > 0.0:
        return (1.0 - auc / (2.0 * tp2 * fp2))
    else:
        return None

class EvalPloter:
    def __init__(self):
        ''' The evaluation plot
        '''
        self.train_iter = []
        self.train_loss = []
        
        self.test_iter = []
        self.test_loss = []
        self.test_accuracy = []
        self.test_auc = []

    def add_train_plot(self, iter, loss):
        self.train_iter.append(iter)
        self.train_loss.append(loss)

    def add_test_plot(self, iter, loss, accuracy=None, auc=None):
        self.test_iter.append(iter)
        self.test_loss.append(loss)
        if accuracy != None:
            self.test_accuracy.append(accuracy)
        if auc != None:
            self.test_auc.append(auc)

    def draw(self, filename):
        fig = plt.figure(figsize=(19, 5))
        fig.subplots_adjust(wspace=0.3)

        # draw train loss
        axis = fig.add_subplot(131)
        self.draw_axis(axis, self.train_iter, self.train_loss, 'Iteration', 'Loss', 'Train Loss')
        
        # draw test loss
        axis = fig.add_subplot(132)
        self.draw_axis(axis, self.test_iter, self.test_loss, 'Iteration', 'Loss', 'Test Loss')

        dpi = 200
        # draw test accuracy
        if len(self.test_accuracy) > 0:
            axis = fig.add_subplot(133)
            self.draw_axis(axis, self.test_iter, self.test_accuracy, 'Iteration', 'Loss', 'Test Accuracy')
            dpi += 100

        # draw test auc
        if len(self.test_auc) > 0:
            axis = fig.add_subplot(134)
            self.draw_axis(axis, self.test_iter, self.test_auc, 'Iteration', 'Auc', 'Test Auc')
            dpi += 100

        plt.savefig(filename, dpi=dpi, bbox_inches='tight')

    def draw_axis(self, axis, x_data, y_data, x_label, y_label, label):
        axis.plot(x_data, y_data, linewidth=2, label=y_label)
        axis.grid(True)
        ttl = axis.set_title(label, fontsize=18, fontweight='bold')
        ttl.set_position([.5, 1.05])
        axis.set_xlabel(x_label, fontsize=12, labelpad=12)
        axis.set_ylabel(y_label, fontsize=12, labelpad=12)
        axis.legend(loc='upper right', fontsize=12)

