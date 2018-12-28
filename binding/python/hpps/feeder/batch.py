""" batch iterator"""
from __future__ import absolute_import

import ctypes
from hpps.base import check_call, LIB, c_str, c_array
from hpps.feeder.tensor import Tensor

class Batch(object):
    """ The BatchIterator
    """
    def __init__(self, handle):
        """ The batch from iterator 
        """
        self.handle = handle

    def __del__(self):
        """ Delete tensor
        """
        check_call(LIB.HPPS_BatchDestroy(self.handle))

    def get_tensor(self, name):
        """ Return the tensor according to name
        """
        out = ctypes.c_void_p()
        check_call(LIB.HPPS_BatchGetTensorFromKey(self.handle,
                                                  c_str(name),
                                                  ctypes.byref(out)))
        return Tensor(handle=out, shape=None, type=None)


