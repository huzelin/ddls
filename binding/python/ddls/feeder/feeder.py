""" Feeder for batch production"""
from __future__ import absolute_import

import ctypes
from hpps.base import check_call, LIB, c_str, c_array
from hpps.feeder.batch_iterator import BatchIterator

class Feeder(object):
    """ The feeder
    """
    def __init__(self):
        """ create a new Tensor instance
        """
        pass

    def start(self, thread_num):
        """ Start feeder,
        Note: now must called after schedule, for dynamic scheduleing should fix it.
        """
        check_call(LIB.HPPS_FeederStart(thread_num))

    def schedule(self, plan, max_queue_size = 1):
        """ Schedule the plan
        """
        out = ctypes.c_void_p()
        check_call(LIB.HPPS_FeederSchedule(plan.handle,
                                           max_queue_size,
                                           ctypes.byref(out)))
        return BatchIterator(out)

    def stop(self):
        """ Stop feeder
        """
        check_call(LIB.HPPS_FeederStop())

