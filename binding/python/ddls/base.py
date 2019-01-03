""" base for ddls """
from __future__ import absolute_import

import sys
import os
import ctypes
import numpy as np
from .libinfo import find_lib_path

#------------------------------------------
# library loading
#------------------------------------------
py_str = lambda x: x

class HPPSError(Exception):
    """ Error that will be throwed by all HPPS functions """
    pass

def _load_lib(name):
    """ Load library by searching possible path. """
    libpath = find_lib_path(name)
    lib = ctypes.CDLL(libpath, ctypes.RTLD_GLOBAL)
    return lib

# library instance of xdl
LIB = _load_lib('ddls')

_NP_2_DTYPE = {
    np.int32 : 0,
    np.int64 : 1,
    np.float32 : 2
}

_DTYPE_2_NP = {
    0 : np.int32,
    1 : np.int64,
    2 : np.float32
}

#-------------------------------------------
# helper function definition
#-------------------------------------------
def check_call(ret):
    """
    Check the return value of C API call

    This function will raise exception when error occurs
    Warp every API call with this function

    Parameters
    ---------
    ret : int
        return value from C API calls
    """
    if ret != 0:
        msg = ctypes.c_char_p(None)
        LIB.HPPS_GetLastErrorString(ctypes.byref(msg))
        raise HPPSError(py_str(msg.value))

def c_str(string):
    """
    Create ctypes char* from a python string

    Parameters:
    ---------
    string : string type
             python string

    Returns:
    ---------
    str: c_char_p
       A char pointer that can be passed to C API
    """
    return ctypes.c_char_p(string.encode('utf-8'))

def c_array(ctype, values=None, count=0):
    """
    Create ctypes array from a python array

    Parameters:
    ----------
    ctype: ctypes data type
        data type of the array we want to convert to
    values: tuple or list
        data content

    Returns:
    ---------
      out : ctypes array
         Created ctypes array
    """
    if values is not None:
        return (ctype * len(values))(*values)
    elif count > 0:
        return (ctype * count)()


