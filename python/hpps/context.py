# coding: utf-8
"""Context management API of ams"""
from __future__ import absolute_import
import ctypes

class Context(object):
    """Constructing a context."""
    devtype2str = {0: 'cpu', 1: 'gpu_pinned',  2: 'gpu'}
    devstr2type = {'cpu': 0, 'gpu_pinned': 1, 'gpu': 2}

    def __init__(self, device_type, device_id=0):
        if isinstance(device_type, Context):
            self.device_typeid = device_type.device_typeid
            self.device_id = device_type.device_id
        else:
            self.device_typeid = Context.devstr2type[device_type]
            self.device_id = device_id

    def device_type(self):
        """Return device type of current context."""
        return Context.devtype2str[self.device_typeid]
    
    def __str__(self):
        return '%s(%d)' % (self.device_type(), self.device_id)

    def __repr__(self):
        return self.__str__()

    def __eq__(self, other):
        """Compare two contexts. Two contexts are equal if they
        have the same device type and device id.
        """
        if not isinstance(other, Context):
            return False
        if self.device_typeid == other.device_typeid and \
                self.device_id == other.device_id:
            return True
        return False

def cpu(device_id=0):
    """Return a CPU context.
    
    This function is a short cut for Context('cpu', device_id)

    :param device_id: The device id of the device. device_id is not needed for CPU.
        This is included to make interface compatible with GPU. (Default value = 0)
    :type device_id: int, optional

    
    """
    return Context('cpu', device_id)


def gpu(device_id=0):
    """Return a GPU context.
    
    This function is a short cut for Context('gpu', device_id)

    :param device_id: The device id of the device, needed for GPU (Default value = 0)
    :type device_id: int, optional

    
    """
    return Context('gpu', device_id)
