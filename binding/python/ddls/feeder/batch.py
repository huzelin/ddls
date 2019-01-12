""" batch iterator"""
from __future__ import absolute_import

import ctypes
from ddls.base import check_call, LIB, c_str, c_array
from ddls.hpps.tensor import Tensor

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

    def names(self):
        """ Return the names of tensor
        """
        out_names = ctypes.POINTER(ctypes.c_char_p)()
        out_size = ctypes.c_int()
        check_call(LIB.HPPS_BatchGetKeys(self.handle,
                                         ctypes.byref(out_size),
                                         ctypes.byref(out_names)))
        return tuple(out_names[:out_size.value]) 

    def add_indices_tensor(self, names_array):
        """ add indices tensor
        """
        check_call(LIB.HPPS_AddIndicesTensor(self.handle,
                                             len(names_array),
                                             c_array(ctypes.c_char_p, names_array)))

    def add_uniqid_tensor(self, names_array):
        """ add uniqid tensor
        """
        check_call(LIB.HPPS_AddUniqIdTensor(self.handle,
                                            len(names_array),
                                            c_array(ctypes.c_char_p, names_array)))



