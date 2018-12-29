from __future__ import absolute_import

import os
import sys
import time
import types
import logging

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
