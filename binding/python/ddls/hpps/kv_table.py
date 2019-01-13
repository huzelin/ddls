""" kv table for hpps """
from __future__ import absolute_import

import ctypes
import numpy as np
from ddls.base import _NP_2_DTYPE, c_str, c_array, check_call, LIB
from ddls.hpps.tensor import Tensor
from ddls.hpps.table import Table

class KVTable(Table):
    """ KVTable is used for sparse kv Parameter. 
    """
    def get(self, key, value):
        """ pull parameter

        Parameters
        ----------
          key: the uint32 or uint64 key tensor
          value: The params tensor
        """
        check_call(LIB.KVTableGet(self.handle, key.handle, value.handle))

    def get_async(self, key, value, wait_id=None):
        """ pull parameter

        Parameters
        ----------
          key: the uint32 or uint64 key tensor
          value: The params tensor
          wait_id: The wait id
        """
        if wait_id is not None:
            self.wait(wait_id)
            check_call(LIB.KVTableGetFromLocal(self.handle, key.handle, value.handle))
        else:
            id = ctypes.c_int()
            check_call(LIB.KVTableGetAsync(self.handle, key.handle, value.handle, ctypes.byref(id)))
            return id

    def add(self, key, grad, option={}):
        """ push grads

        Parameters
        ----------
          key: the uint32 or uint64 key tensor
          grad: The param grads
        """
        keys = []
        values = []
        for key, value in option.iteritems():
            keys.append(c_str(key))
            values.append(c_str(value))
        check_call(LIB.KVTableAdd(self.handle, key.handle, grad.handle,
                                  c_array(ctypes.c_char_p, keys),
                                  c_array(ctypes.c_char_p, values)))

    def add_async(self, key, grad, option={}):
        """ push grads

        Parameters
        ----------
          key: the uint32 or uint64 key tensor
          grad: The param grads
        """
        keys = []
        values = []
        for key, value in option.iteritems():
            keys.append(c_str(key))
            values.append(c_str(value))
        id = ctypes.c_int()
        check_call(LIB.KVTableGetAsync(self.handle, key.handle, grad.handle, ctypes.byref(id),
                                       c_array(ctypes.c_char_p, keys),
                                       c_array(ctypes.c_char_p, values)))
        return id

# create kv table
def create_kv_table(capacity, value_len, key_type, value_type, solver = "", ps_mode = "async", kwargs = { }):
    """ create a new kv table

    Parameters
    ---------
      capacity:
         The key capacity
      value_len:
         The value length
      key_type:
         The key type
      value_type:
         The value type
      solver:
         The solver
      ps_mode:
         The ps model(sync/async)

      kwargs:
         The k/w config, The available key as follows:

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
    check_call(LIB.CreateKVTable(c_str(solver),
                                 c_str(ps_mode),
                                 capacity,
                                 value_len,
                                 _NP_2_DTYPE[key_type],
                                 _NP_2_DTYPE[value_type],
                                 len(keys),
                                 c_array(ctypes.c_char_p, keys),
                                 c_array(ctypes.c_char_p, values),
                                 ctypes.byref(handle)))
    return KVTable(handle)

