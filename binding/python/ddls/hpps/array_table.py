""" array table for hpps """
from __future__ import absolute_import

import ctypes
import numpy as np
from hpps.base import _NP_2_DTYPE, c_str, c_array, check_call, LIB
from hpps.feeder.tensor import Tensor

class ArrayTable(object):
    """ ArrayTable is used for continuous Parameter. 
    """
    def __init__(self, handle):
        self.handle = handle

    def get(self, tensor):
        """ pull parameter

        Parameters
        ----------
          tensor: The array-like params
        """
        check_call(LIB.ArrayTableGet(self.handle, tensor.handle))

    def get_async(self, tensor):
        """ pull parameter

        Parameters
        ----------
          tensor: The array-like params
        """
        id = ctypes.c_int()
        check_call(LIB.ArrayTableGetAsync(self,handle, tensor.handle, ctypes.byref(id)))
        return id

    def add(self, grad):
        """ push grads

        Parameters
        ----------
          tensor: The array-like grads
        """
        check_call(LIB.ArrayTableAdd(self.handle, grad.handle))

    def add_async(self, grad):
        """ push grads

        Parameters
        ----------
          tensor: The array-like grads
        """
        id = ctypes.c_int()
        check_call(LIB.ArrayTableGetAsync(self.handle, grad.handle, ctypes.byref(id)))
        return id

    def wait(self, id):
        """ wait async pull or push

        Parameters
        ----------
          id: The async pull or push's return value
        """
        check_call(LIB.TableWait(self.handle, id))

# create array table
def create_array_table(size, type, kwargs = { }):
    """ create a new array table

    Parameters
    ---------
      size:
         The array length
      type:
         The array data type
      kwargs:
         The k/w config, for example:

         algo : assign/uniform/gaussian
         assigned_value : 0.1
         mu : 0
         sigma : 0.01
         min : 0
         max : 1
         seed : 0

    """
    keys = []
    values = []
    for key, value in kwargs.iteritems():
        keys.append(c_str(key))
        values.append(c_str(value))
    handle = ctypes.c_void_p()
    check_call(LIB.CreateArrayTable(size,
                                    _NP_2_DTYPE[type],
                                    len(keys),
                                    c_array(ctypes.c_char_p, keys),
                                    c_array(ctypes.c_char_p, values),
                                    ctypes.byref(handle)))
    return ArrayTable(handle)

