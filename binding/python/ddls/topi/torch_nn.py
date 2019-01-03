from __future__ import absolute_import

import sys
import os
import ctypes
import torch
import numpy as np

from hpps.topi.nn import ModelParamManager 

class TorchParamManager(ModelParamManager):
    """
    TorchParamManager is a manager for synchroning the variables in PyTorch more easily
    """
    def get_all_arg_list(self):
        """ Get all args map of specific model

        Parameters
        ----------
          None

        Return
        ------
          A list of Numpy.
        """
        ret = [ ]
        for param in self.model.parameters():
            if param.requires_grad:
                ret.append(param.data.numpy())
        return ret

    def get_all_aux_list(self):
        """ Get all auxs list of specific model

        Parameters
        ----------
          None

        Return
        ------
          A list of Numpy.
        """
        ret = [ ]
        for param in self.model.buffers():
            ret.append(param.data.numpy())
        return ret

    def get_all_arg_grad_list(self):
        """ Get all arg grad list of specific model

        Parameters
        ----------
          None

        Return
        ------
          A list of Numpy.
        """
        ret = [ ]
        for param in self.model.parameters():
            if param.requires_grad:
                ret.append(param.grad.data.numpy())
        return ret
    
    def zero_grad(self):
        """ zero all grad 

        Parameters
        ----------
          None

        Return
        ------
          A list of Numpy.
        """
        ret = [ ]
        for param in self.model.parameters():
            if param.requires_grad:
                if param.grad is not None:
                    param.grad.data.zero_()
        return ret

    def set_all_arg_to_model(self):
        """ Set all args to specific model

        Parameters
        ----------
          None

        Return
        ------
          None
        """
        arg_np = self.arg_tensor.asnumpy()
        for index in xrange(len(self.arg_offsets)):
            start = self.arg_offsets[index]
            if index + 1 == len(self.arg_offsets):
                end = self.arg_size
            else:
                end = self.arg_offsets[index + 1]

            tmp = arg_np[start : end].reshape(self.arg_shapes[index])
            list(self.model.parameters())[index].data = torch.from_numpy(tmp)
    
    def set_all_aux_to_model(self):
        """ Set all aux to specific model

        Parameters
        ----------
          None

        Return
        ------
          None
        """
        aux_np = self.aux_tensor.asnumpy()
        for index in xrange(len(self.aux_offsets)):
            start = self.aux_offsets[index]
            if index + 1 == len(self.aux_offsets):
                end = self.aux_size
            else:
                end = self.aux_offsets[index + 1]

            tmp = aux_np[start : end].reshape(self.aux_shapes[index])
            list(self.model.parameters())[index].data = torch.from_numpy(aux_np)

    def save_model(self, path, dummy_input, inames, onames):
        """ Save model

        Parameters
        ----------
          path: The model path
          dummy_input: The dummy input
          inames: The input names
          onames: The output names

        Return
        ------
          None
        """
        torch.onnx.export(self.model,
                          dummy_input,
                          path,
                          verbose=True,
                          input_names=inames,
                          output_names=onames)
 
