from __future__ import absolute_import

import sys
import os
import ctypes
import numpy as np

from hpps.array_table import ArrayTable, create_array_table
from hpps.feeder.tensor import Tensor
from hpps.zoo import Zoo

class ModelParamManager(object):
    """ The neural network model param manager, which is used for managing and
    synchronizing the variables in NN model more easily
    """
    def __init__(self, model):
        """ The constructor of ModelParamManager

        The constructor will associate the parameter with hpps array table,
        the initial value of ArrayTable will be same as the parameter of model.
        If different parameters are used in different process, the average of them
        will be used as the initial value.

        """
        self.model = model
        
        self.arg_shapes = []
        self.arg_dtypes = []
        self.arg_sizes = []

        self.aux_shapes = []
        self.aux_dtypes = []
        self.aux_sizes = []

        for arr in self.get_all_arg_list():
            self.arg_shapes.append(arr.shape)
            self.arg_dtypes.append(arr.dtype)
            self.arg_sizes.append(arr.size)

        for arr in self.get_all_aux_list():
            self.aux_shapes.append(arr.shape)
            self.aux_dtypes.append(arr.dtype)
            self.aux_sizes.append(arr.size)

        z = Zoo()
        self.arg_tensor = None
        self.arg_grad_tensor = None

        self.arg_array_table = create_array_table(sum(self.arg_sizes), np.float32)
        if z.is_worker():
            self.arg_tensor = Tensor([sum(self.arg_sizes)], np.float32)
            seld.arg_grad_tensor = Tensor([sum(self.arg_sizes)], np.float32)
        
        self.aux_tensor = None
        if len(self.aux_sizes) > 0:
            self.aux_array_table = create_array_table(sum(self.aux_sizes), np.float32)
            if z.is_worker():
                self.aux_tensor = Tensor([sum(self.aux_sizes)], np.float32)
        
        z.barrier()

        if z.is_worker():
            self.arg_array_table.get(self.arg_tensor)
            if len(self.aux_sizes) > 0:
                self.aux_array_table.get(self.aux_tensor)
            
            self.set_all_arg_to_model()
            self.set_all_aux_to_model()

    def get_all_arg_list(self):
        """ Get all args of specific model

        Parameters
        ----------
          None

        Return
        ------
          A list of Numpy.
        """
        raise NotImplemented()

    def get_all_aux_list(self):
        """ Get all auxs of specific model

        Parameters
        ----------
          None

        Return
        ------
          A list of Numpy.
        """
        raise NotImplemented()

    def get_all_arg_grad_list(self):
        """ Get all arg grad of specific model

        Parameters
        ----------
          None

        Return
        ------
          A list of Numpy.
        """
        raise NotImplemented()

    def set_all_arg_to_model(self):
        """ Set all args to specific model

        Parameters
        ----------
          None

        Return
        ------
          None
        """
        raise NotImplemented()

    def set_all_aux_to_model(self):
        """ Set all aux to specific model

        Parameters
        ----------
          None

        Return
        ------
          None
        """
        raise NotImplemented()

    def sync_all_param(self):
        """ Sync all params

        Parameters
        ----------
          None

        Return
        ------
          None
        """
        if z.is_worker() == False:
            return

        for arr in self.get_all_arg_grad_list():
            # Copy grad
            pass
        
        self.arg_array_table.add(self.arg_grad_tensor)
        self.arg_array_table.get(self.arg_tensor)
        self.set_all_arg_to_model()

        if len(self.aux_sizes) > 0:
            self.aux_array_table.add(self.arg_aux_tensor)
            self.aux_array_table.get(self.arg_aux_tensor)
            self.set_all_aux_to_model()

