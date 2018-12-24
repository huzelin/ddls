"""tensor for ams"""
from __future__ import absolute_import

import ctypes
import os
import numpy as np

from .base import LIB, check_call, c_str, c_array
from .context import Context
from .data_type import amsdtype2numpy, amsdtype2string, numpy2amsdtype

class Tensor(object):
    """Tensor operation in ams."""
    def __init__(self, shape, ctx, dtype=np.float32, dptr=None):
        self.handle = ctypes.c_void_p()
        check_call(LIB.AMS_TensorCreate(len(shape),
                                       c_array(ctypes.c_int, shape),
                                       ctx.device_typeid,
                                       ctx.device_id,
                                       numpy2amsdtype[dtype],
                                       dptr,
                                       ctypes.byref(self.handle)))

    def __del__(self):
        check_call(LIB.AMS_TensorDelete(self.handle))

    def __repr__(self):
        return ""

    def asnumpy(self):
        """Return a copied numpy array of current tensor


        :returns: array: numpy.ndarray
        A copy of array content

        """
        data = np.empty(self.shape, dtype=amsdtype2numpy[self.dtype])
        check_call(LIB.AMS_TensorExport(self.handle,
                                       data.ctypes.data_as(ctypes.c_void_p),
                                       ctypes.c_size_t(data.size)))
        return data
    
    def load_numpy(self, data):
        """Load from numpy

        :param data: 

        """
        check_call(LIB.AMS_TensorImport(self.handle,
                                        data.ctypes.data_as(ctypes.c_void_p),
                                        ctypes.c_size_t(data.size)))

    @property
    def shape(self):
        """ """
        ndim = ctypes.c_size_t()
        pdata = ctypes.POINTER(ctypes.c_size_t)()
        check_call(LIB.AMS_TensorGetShape(self.handle, ctypes.byref(ndim), ctypes.byref(pdata)))
        return tuple(pdata[:ndim.value])

    @property
    def dptr(self):
        """ """
        data = ctypes.c_void_p()
        check_call(LIB.AMS_TensorGetData(self.handle, ctypes.byref(data)))
        return data

    @property
    def context(self):
        """ """
        dev_typeid = ctypes.c_int()
        dev_id = ctypes.c_int()
        check_call(LIB.AMS_TensorGetContext(self.handle, ctypes.byref(dev_typeid), ctypes.byref(dev_id)))
        return Context(Context.devtype2str[dev_typeid.value], dev_id.value)

    @property
    def dtype(self):
        """ """
        dtype = ctypes.c_int()
        check_call(LIB.AMS_TensorGetType(self.handle, ctypes.byref(dtype)))
        return dtype.value

    def reshape(self, new_shape):
        """Reshape the tensor

        :param new_shape: 

        """
        check_call(LIB.AMS_TensorReshape(self.handle, len(new_shape), c_array(ctypes.c_int, new_shape)))
