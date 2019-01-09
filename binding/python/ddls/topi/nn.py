from __future__ import absolute_import

import sys
import os
import ctypes
import numpy as np

from ddls.hpps.tensor import Tensor
from ddls.hpps.zoo import *
from ddls.hpps.array_table import ArrayTable, create_array_table

class ModelParamManager(object):
    """ The neural network model param manager, which is used for managing and
    synchronizing the variables in NN model more easily
    """
    def __init__(self, model, kwargs = { }):
        """ The constructor of ModelParamManager

        The constructor will associate the parameter with hpps array table,
        the initial value of ArrayTable will be same as the parameter of model.
        If different parameters are used in different process, the average of them
        will be used as the initial value.

        """
        self.model = model
        
        self.arg_offsets = []
        self.arg_shapes = []
        self.aux_offsets = []
        self.aux_shapes = []

        self.arg_size = 0
        for value in self.get_all_arg_list():
            assert(np.dtype("float32") == value.dtype)
            self.arg_offsets.append(self.arg_size)
            self.arg_size += value.size
            self.arg_shapes.append(value.shape)

        self.aux_size = 0
        for value in self.get_all_aux_list():
            assert(np.dtype("float32") == value.dtype)
            self.aux_offsets.append(self.aux_size)
            self.aux_size += value.size
            self.aux_shapes.append(value.shape)

        self.arg_tensor = None
        self.arg_grad_tensor = None

        self.arg_array_table = create_array_table(self.arg_size, np.float32, kwargs)
        if zoo_is_worker():
            self.arg_tensor = Tensor([ self.arg_size ], np.float32)
            self.arg_grad_tensor = Tensor([ self.arg_size ], np.float32)
        
        self.aux_tensor = None
        if self.aux_size > 0:
            self.aux_array_table = create_array_table(self.aux_size, np.float32, kwargs)
            if z.is_worker():
                self.aux_tensor = Tensor([ self.aux_size ], np.float32)
        
        zoo_barrier()

        # Pull argument from Parameter Server
        if zoo_is_worker():
            self.arg_array_table.get(self.arg_tensor)
            self.set_all_arg_to_model()
            if self.aux_size > 0:
                self.aux_array_table.get(self.aux_tensor)
                self.set_all_aux_to_model()

    def get_all_arg_list(self):
        """ Get all args list of specific model

        Parameters
        ----------
          None

        Return
        ------
          A list of Numpy.
        """
        raise NotImplemented()

    def get_all_aux_list(self):
        """ Get all auxs list of specific model

        Parameters
        ----------
          None

        Return
        ------
          A list of Numpy.
        """
        raise NotImplemented()

    def get_all_arg_grad_list(self):
        """ Get all arg grad list of specific model

        Parameters
        ----------
          None

        Return
        ------
          A list of Numpy.
        """
        raise NotImplemented()

    def zero_grad(self):
        """ zero all grad

        Parameters
        ----------
          None

        Return
        ------
          None
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
        if zoo_is_worker() == False:
            return

        # copy grad from backend engine
        all_arg_grad_list = self.get_all_arg_grad_list()
        for index in xrange(len(all_arg_grad_list)):
            self.arg_grad_tensor.load_numpy(all_arg_grad_list[index],
                                            self.arg_offsets[index])
        
        # push grad and pull arg
        self.arg_array_table.add(self.arg_grad_tensor)
        self.arg_array_table.get(self.arg_tensor)

        # deploy new arg to backend engine
        self.set_all_arg_to_model()

        if self.aux_size > 0:
            # copy aux from backend engine
            all_aux_list = self.get_all_aux_list()
            for index in xrange(len(all_aux_list)):
                self.aux_tensor.load_numpy(all_aux_list[index], self.aux_offsets[index])

            # push and pull aux 
            self.aux_array_table.add(self.arg_aux_tensor)
            self.aux_array_table.get(self.arg_aux_tensor)

            # deploy new aux to backend engine
            self.set_all_aux_to_model()

    def save_model(self, path, dummy_input, inames, onames):
        """ Save model

        Parameters
        ----------
          path: The file path of model
          dummy_input: The dummpy input
          inames: The input names
          onames: The output names

        Return
        ------
          None
        """
        raise NotImplemented()
 
