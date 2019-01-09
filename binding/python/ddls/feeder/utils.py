""" utils for the tensor"""
from __future__ import absolute_import

import ctypes
import numpy as np
from ddls.hpps.tensor import Tensor

def Num2Indices(num_tensor):
    """ Convert num tensor to indices.

    Parameters
    ----------
      num_tensor: The num tensor
    """
    out = ctypes.c_void_p()
    check_call(LIB.Num2Indices(num_tensor.handle,
                               ctypes.byref(out)))
    return Tensor(handle = out, own_handle = True)
