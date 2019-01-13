from __future__ import absolute_import

import sys
import os
import ctypes
import numpy as np

from ddls.hpps.tensor import Tensor
from ddls.hpps.zoo import *
from ddls.hpps.kv_table import KVTable, create_kv_table
from ddls.feeder.batch import Batch

class Embedding(object):
    """ The embedding model param manager, which is used for managing and
    synchronizing the variables in sparse embedding.
    """
    def __init__(self, capacity, value_len, kwargs = { }, solver='adam'):
        """ The constructor of Embedding
        """
        self.kv_table = create_kv_table(capacity=capacity,
                                        value_len=value_len,
                                        key_type=np.int64,
                                        value_type=np.float32,
                                        kwargs=kwargs,
                                        solver=solver)
        self.wait_get_id = None
        self.wait_add_id = None

        self.value = Tensor([1], np.float32)
        self.grad = Tensor([1], np.float32)

    def get(self, key, next_key):
        """ get current key's value and pre-get the next key.

        Parameters
        ----------
          key: The current iteration id key, which is Tensor instance
          next_key: The next iteration id key

        Returns
        -------
          Return value, which is numpy instance.
        """
        assert isinstance(key, Tensor)
        assert isinstance(next_key, Tensor)

        if self.wait_get_id is None:
            self.wait_get_id = self.kv_table.get_async(key=key, value=self.value)

        self.kv_table.get_async(key=key, value=self.value, wait_id=self.wait_get_id)
        self.wait_get_id = self.kv_table.get_async(key=next_key, value=self.value)
        return self.value.asnumpy()

    def add(self, key, grad, option={}):
        """ add grad

        Parameter
        ---------
          key: The current iteration id key, which is Tensor instance
          grad: The current iteration id grad, which is numpy instance
        """
        assert isinstance(key, Tensor)

        if self.wait_add_id is not None:
            self.kv_table.wait(self.wait_add_id)

        self.grad.reshape(grad.shape)
        self.grad.load_numpy(grad)

        self.wait_add_id = self.kv_table.add_async(key=key, grad=self.grad, option=option)

