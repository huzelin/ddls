""" Sample reading plan"""
from __future__ import absolute_import

import ctypes
from hpps.base import check_call, LIB, c_str
from hpps.feeder.plan import Plan

class PlanMaker(Object):
    def __init__(self):
        self.handle = ctypes.c_void_p()
        check_call(LIB.HPPS_CreatePlanMaker(ctypes.byref(self.handle)))

    def set_uri(self, files):
        array = []
        for file in files:
            array.append(c_str(file))
        check_call(LIB.HPPS_PlanMakerSetURI(
            self.handle, len(array), c_array(ctypes.c_char_p, array)))

    def set_epoch(self, epoch):
        check_call(LIB.HPPS_PlanMakerSetEpoch(self.handle, epoch))

    def set_batch_size(self, batch_size):
        check_call(LIB.HPPS_PlanMakerSetBatchSize(self.handle, batch_size))

    def make(self):
        nhandle = ctypes.c_void_p();
        check_call(LIB.HPPS_PlanMakerMake(self.handle, ctypes.byref(nhandle)))
        return Plan(nhandle) 
