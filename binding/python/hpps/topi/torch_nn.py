from __future__ import absolute_import

import sys
import os
import ctypes
import torch
import numpy as np

from hpps.topi.nn import ModelParamManager 

class TorchParamManagert(ModelParamManager):
    """
    TorchParamManager is a manager for synchroning the variables in PyTorch more easily
    """
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

