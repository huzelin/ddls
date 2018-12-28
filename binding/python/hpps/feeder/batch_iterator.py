""" batch iterator"""
from __future__ import absolute_import

import ctypes
from hpps.base import check_call, LIB, c_str, c_array
from hpps.feeder.batch import Batch

class BatchIterator(object):
    """ The BatchIterator
    """
    def __init__(self, handle):
        """ The feeder schedule's Iterator
        """
        self.handle = handle

    def next_batch(self):
        """ Return the next batch
        """
        out = ctypes.c_void_p()
        check_call(LIB.HPPS_BatchIteratorPop(self.handle,
                                             ctypes.byref(out)))
        return Batch(out)


