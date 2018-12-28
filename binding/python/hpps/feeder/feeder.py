""" feeder of hpps"""
from __future__ import absolute_import

import ctypes

class Feeder(Object):
    def __init__(self, handle):
        self.handle = handle 

    def handle(self):
        return self.handle
