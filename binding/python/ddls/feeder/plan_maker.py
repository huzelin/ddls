""" Sample reading plan"""
from __future__ import absolute_import

import ctypes
from hpps.base import check_call, LIB, c_str, c_array
from hpps.feeder.plan import Plan

class PlanMaker(object):
    """ The sample reading plan maker 
    """
    def __init__(self):
        """ init a new PlanMaker instance
        """
        self.handle = ctypes.c_void_p()
        check_call(LIB.HPPS_PlanMakerCreate(ctypes.byref(self.handle)))

    def set_uri(self, files):
        """
        set the sample files for hpps

        Parameters
        ---------
          files: The files in array
        """
        array = []
        for file in files:
            array.append(c_str(file))
        check_call(LIB.HPPS_PlanMakerSetURI(
            self.handle, len(array), c_array(ctypes.c_char_p, array)))

    def set_epoch(self, epoch):
        """
        set epoch number of sample files

        Parameters
        ----------
          epoch: The epoch number
        """
        check_call(LIB.HPPS_PlanMakerSetEpoch(self.handle, epoch))

    def set_batch_size(self, batch_size):
        """
        set batch size

        Parameters
        ----------
          batch_size: The batch size
        """
        check_call(LIB.HPPS_PlanMakerSetBatchSize(self.handle, batch_size))

    def make(self):
        """
        make the sample reading plan

        Parameters
        ----------
          None

        Return
        ------
          Return the sample reading plan.
        """
        nhandle = ctypes.c_void_p();
        check_call(LIB.HPPS_PlanMakerMake(self.handle, ctypes.byref(nhandle)))
        return Plan(nhandle) 
