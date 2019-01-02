""" zoo is the controll center of hpps."""
from __future__ import absolute_import

import sys
import os
import ctypes
from ddls.base import LIB, check_call, c_str

#------------------------------------------
# hpps zoo
#------------------------------------------
def zoo_start(args = '-net_type=mpi -sync=true'):
    """Start the Zoo, all the Actors will be registered.
    """
    check_call(LIB.HPPS_ZooStart(c_str(args)))

def zoo_stop():
    """Stop the Zoo, all the Actors will be destroyed
    """
    check_call(LIB.HPPS_ZooStop())

def zoo_barrier():
    """Barrier all the nodes"""
    check_call(LIB.HPPS_ZooBarrier())

def zoo_rank():
    """returns: The rank of node
    """
    rank = ctypes.c_int()
    check_call(LIB.HPPS_ZooRank(ctypes.byref(rank)))
    return rank.value

def zoo_size():
    """:returns: The size of nodes
    """
    size = ctypes.c_int()
    check_call(LIB.HPPS_ZooSize(ctypes.byref(size)))
    return size.value
    
def zoo_num_workers():
    """:returns: the number of workers
    """
    num_workers = ctypes.c_int()
    check_call(LIB.HPPS_ZooNumWorkers(ctypes.byref(num_workers)))
    return num_workers.value

def zoo_num_servers():
    """:returns: the number of servers
    """
    num_servers = ctypes.c_int()
    check_call(LIB.HPPS_ZooNumServers(ctypes.byref(num_servers)))
    return num_servers.value

def zoo_is_worker():
    """:returns: whether the node is worker role.
    """
    is_worker = ctypes.c_int()
    check_call(LIB.HPPS_ZooIsWorker(ctypes.byref(is_worker)))
    return True if is_worker else False

def zoo_is_server():
    """:returns: whether the node is server role.
    """
    is_server = ctypes.c_int()
    check_call(LIB.HPPS_ZooIsServer(ctypes.byref(is_server)))
    return True if is_server else False

def zoo_is_controller():
    """:returns: whether the node is controller role.
    """
    is_controller = ctypes.c_int()
    check_call(LIB.HPPS_ZooIsController(ctypes.byref(is_controller)))
    return True if is_controller else False

def zoo_worker_id():
    """:returns: the worker id [0, num_workers())
    """
    worker_id = ctypes.c_int()
    check_call(LIB.HPPS_ZooWorkerId(ctypes.byref(worker_id)))
    return worker_id.value

def zoo_server_id():
    """:returns: the server id [0, num_servers())
    """
    server_id = ctypes.c_int()
    check_call(LIB.HPPS_ZooServerId(ctypes.byref(server_id)))
    return server_id.value

def zoo_set_log_level(log_level = 2):
    """

    Parameters:
        -----------

        log_level: 0 -> Debug 
                   1 -> Info
                   2 -> Error
                   3 -> Fatal
    """
    check_call(LIB.HPPS_ZooSetLogLevel(log_level))

def zoo_set_log_file(log_file):
    """

        Parameters
        ----------

        log_file: The log file path for HPPS.

    """
    check_call(LIB.HPPS_ZooSetLogFile(c_str(log_file)))

