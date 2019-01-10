""" table for hpps """
from __future__ import absolute_import

import ctypes
import numpy as np
from ddls.base import _NP_2_DTYPE, c_str, c_array, check_call, LIB
from ddls.hpps.tensor import Tensor

class Table(object):
    """ Table base. 
    """
    def __init__(self, handle):
        self.handle = handle

    def wait(self, id):
        """ wait async pull or push

        Parameters
        ----------
          id: The async pull or push's return value
        """
        check_call(LIB.TableWait(self.handle, id))

    def load(self, uri):
        """ load table

        Parameters
        ----------
          uri: The uri name
        """
        check_call(LIB.TableLoad(self.handle, c_str(uri)))

    def store(self, uri):
        """ store table

        Parameters
        ----------
          uri: The uri name
        """
        check_call(LIB.TableStore(self.handle, c_str(uri)))

