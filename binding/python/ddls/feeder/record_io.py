""" RecordIO for sample file production"""
from __future__ import absolute_import

import ctypes
from ddls.base import check_call, LIB, c_str, c_array, _NP_2_DTYPE
from ddls.hpps.tensor import Tensor

# FileOpenMode
BinaryWrite = 3
BinaryRead = 4

class RecordIO(object):
    """ The sample record i/o 
    """
    def __init__(self, uri, mode):
        """ init a new RecordIO instance
        """
        self.handle = ctypes.c_void_p()
        check_call(LIB.HPPS_RecordIOCreate(c_str(uri), mode, ctypes.byref(self.handle)))

    def __del__(self):
        """ delete RecordIO instance
        """
        check_call(LIB.HPPS_RecordIODestroy(self.handle))

    def write_header(self, name_type_map):
        """
        write header of record

        Parameters
        ---------
          name_type_map: tensor name -> type map
        """
        names = []
        types = []
        for name, type in name_type_map.iteritems():
            names.append(c_str(name))
            types.append(_NP_2_DTYPE[type])
        check_call(LIB.HPPS_RecordIOWriteHeader(self.handle,
                                                len(names),
                                                c_array(ctypes.c_char_p, names),
                                                c_array(ctypes.c_int, types)))

    def write_sample(self, tensor_map):
        """
        write one sample

        Parameters
        ----------
          tensor_map: The name -> tensor map, each sample consists of mutiple tensors
        """
        names = []
        tensors = []
        for name, tensor in tensor_map.iteritems():
            names.append(c_str(name))
            tensors.append(tensor.handle)
        check_call(LIB.HPPS_RecordIOWriteSample(self.handle,
                                                len(names),
                                                c_array(ctypes.c_char_p, names),
                                                c_array(ctypes.c_void_p, tensors)))

    def write_finalize(self):
        """
        record io write finalization

        Parameters
        ----------
          None

        """
        check_call(LIB.HPPS_RecordIOWriteFinalize(self.handle))
