""" utils for the tensor"""
from __future__ import absolute_import

import ctypes
import numpy as np
from ddls.base import check_call, LIB
from ddls.hpps.tensor import Tensor

def Num2Indices(num_tensor):
    """ Convert num tensor to indices.

    Parameters
    ----------
      num_tensor: The num tensor
    """
    out = ctypes.c_void_p()
    check_call(LIB.HPPS_Num2Indices(num_tensor.handle,
                                    ctypes.byref(out)))
    return Tensor(handle = out, own_handle = True, shape = None, type = None)

def Id2UniqId(id_tensor):
    """ Convert id tensor to [uniq_id, local_id]

    Parameters
    ----------
      id_tensor: The id tensor
    """
    local_id = ctypes.c_void_p()
    uniq_id = ctypes.c_void_p()
    check_call(LIB.HPPS_Id2UniqId(id_tensor.handle,
                                  ctypes.byref(local_id),
                                  ctypes.byref(uniq_id)))
    return [
             Tensor(handle = local_id, own_handle = True, shape = None, type = None),
             Tensor(handle = uniq_id, own_handle = True, shape = None, type = None)
           ]
