""" RecordIO for sample file production"""
from __future__ import absolute_import

import ctypes
import numpy as np
from hpps.base import check_call, LIB, c_str, c_array, _NP_2_DTYPE, _DTYPE_2_NP

class Tensor(object):
    """ The sample record i/o 
    """
    def __init__(self, shape, type, handle = None, own_handle = False):
        """ create a new Tensor instance
        """
        if handle != None:
            self.own_handle = own_handle
            self.handle = handle
        else:
            self.own_handle = True
            self.handle = ctypes.c_void_p()
            check_call(LIB.HPPS_TensorCreate(len(shape),
                                             c_array(ctypes.c_int, shape),
                                             _NP_2_DTYPE[type],
                                             ctypes.byref(self.handle)))

    def __del__(self):
        """ delete Tensor instance
        """
        if self.own_handle:
            check_call(LIB.HPPS_TensorDestroy(self.handle))

    def __repr__(self):
        """ Return the string representation of the tensor
        """
        shape_info = 'x'.join(['%d' % x for x in self.shape])
        return '<%s %s @%s >' % (self.__class__.__name__,
                              shape_info,
                              self.dtype)

    @property
    def shape(self):
        """ Return the shape of Tensor
        """
        ndim = ctypes.c_uint()
        pdata = ctypes.POINTER(ctypes.c_uint)()
        check_call(LIB.HPPS_TensorShape(self.handle, ctypes.byref(ndim), ctypes.byref(pdata)))
        return tuple(pdata[:ndim.value])

    @property
    def dptr(self):
        """ Return the dptr of Tensor
        """
        data = ctypes.c_void_p()
        check_call(LIB.HPPS_TensorData(self.handle, ctypes.byref(data)))
        return data

    @property
    def dtype(self):
        """ Return the dtype
        """
        dtype = ctypes.c_ubyte()
        check_call(LIB.HPPS_TensorType(self.handle, ctypes.byref(dtype)))
        return _DTYPE_2_NP[dtype.value]

    def load_numpy(self, data, offset = 0):
        """ load data from numpy
        """
        check_call(LIB.HPPS_TensorLoadData(self.handle,
                                           offset,
                                           data.ctypes.data_as(ctypes.c_void_p),
                                           data.size))

    def asnumpy(self):
        """ Return the numpy
        """
        data = np.empty(self.shape, dtype=self.dtype)
        check_call(LIB.HPPS_TensorExportData(self.handle,
                                             data.ctypes.data_as(ctypes.c_void_p)))
        return data

