"""data type for ams"""
from __future__ import absolute_import

import sys
import numpy as np

#-------------------------------------
# The supported data type
#-------------------------------------
np.int72 = np.dtype([('id_grp', np.uint8),  ('id', np.uint64)])
np.int80 = np.dtype([('id_grp', np.uint16), ('id', np.uint64)])

numpy2amsdtype = {
    np.float16 : 0,
    np.float32 : 1,
    np.float64 : 2,
    np.uint64 : 3,
    np.uint32 : 4,
    np.int32 : 5,
    np.uint16 : 6,
    np.int16 : 7,
    np.uint8 : 8,
    np.int8 : 9,
    np.int72 : 10,
    np.int80 : 11,
}

amsdtype2numpy = {
    0 : np.float16,
    1 : np.float32,
    2 : np.float64,
    3 : np.uint64,
    4 : np.uint32,
    5 : np.int32,
    6 : np.uint16,
    7 : np.int16,
    8 : np.uint8,
    9 : np.int8,
    10 : np.int72,
    11 : np.int80,
}

amsdtype2string = {
    0 : "float16",
    1 : "float32",
    2 : "float64",
    3 : "uint64",
    4 : "uint32",
    5 : "int32",
    6 : "uint16",
    7 : "int16",
    8 : "uint8",
    9 : "int8",
    10 : "int72",
    11 : "int80",
}
