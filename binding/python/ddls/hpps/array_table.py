""" array table for hpps """
from __future__ import absolute_import

import ctypes
import numpy as np
from ddls.base import _NP_2_DTYPE, c_str, c_array, check_call, LIB
from ddls.hpps.tensor import Tensor

class ArrayTable(object):
    """ ArrayTable is used for continuous Parameter. 
    """
    def __init__(self, handle):
        self.handle = handle

    def get(self, value):
        """ pull parameter

        Parameters
        ----------
          value: The array-like params
        """
        check_call(LIB.ArrayTableGet(self.handle, value.handle))

    def get_async(self, value):
        """ pull parameter

        Parameters
        ----------
          value: The array-like params
        """
        id = ctypes.c_int()
        check_call(LIB.ArrayTableGetAsync(self,handle, value.handle, ctypes.byref(id)))
        return id

    def add(self, grad, option={}):
        """ push grads

        Parameters
        ----------
          tensor: The array-like grads
        """
        keys = []
        values = []
        for key, value in option.iteritems():
            keys.append(c_str(key))
            values.append(c_str(value))
        check_call(LIB.ArrayTableAdd(self.handle, grad.handle,
                                     len(keys),
                                     c_array(ctypes.c_char_p, keys),
                                     c_array(ctypes.c_char_p, values)))

    def add_async(self, grad, option={}):
        """ push grads

        Parameters
        ----------
          tensor: The array-like grads
        """
        keys = []
        values = []
        for key, value in option.iteritems():
            keys.append(c_str(key))
            values.append(c_str(value))
        id = ctypes.c_int()
        check_call(LIB.ArrayTableGetAsync(self.handle, grad.handle, ctypes.byref(id),
                                          len(keys),
                                          c_array(ctypes.c_char_p, keys),
                                          c_array(ctypes.c_char_p, values)))
        return id

    def wait(self, id):
        """ wait async pull or push

        Parameters
        ----------
          id: The async pull or push's return value
        """
        check_call(LIB.TableWait(self.handle, id))

# create array table
def create_array_table(size, type, solver = "", ps_mode = "sync", kwargs = { }):
    """ create a new array table

    Parameters
    ---------
      size:
         The array length
      type:
         The array data type
      solver:
         The user defined solver
      ps_mode:
         ps mode(sync/async) 
      kwargs:
         The k/w config, The available keys as follows::

           algo :
             assign/uniform/gaussian
           assigned_value :
             0.1
           mu :
             0
           sigma :
             0.01
           min :
             0
           max :
             1
           seed :
             0
    """
    keys = []
    values = []
    for key, value in kwargs.iteritems():
        keys.append(c_str(key))
        values.append(c_str(value))
    handle = ctypes.c_void_p()
    check_call(LIB.CreateArrayTable(c_str(solver),
                                    c_str(ps_mode),
                                    size,
                                    _NP_2_DTYPE[type],
                                    len(keys),
                                    c_array(ctypes.c_char_p, keys),
                                    c_array(ctypes.c_char_p, values),
                                    ctypes.byref(handle)))
    return ArrayTable(handle)

