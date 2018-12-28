""" Sample reading plan"""
from __future__ import absolute_import

import ctypes

class Plan(Object):
    def __init__(self, handle):
        self.handle = handle 

    def handle(self):
        return self.handle
