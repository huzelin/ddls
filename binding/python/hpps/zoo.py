""" zoo is the controll center of hpps."""
from __future__ import absolute_import

import sys
import os
import ctypes
from .base import LIB, check_call, c_str

#------------------------------------------
# hpps zoo
#------------------------------------------
class Zoo(object):
    """The hpps zoo, which use Actor Model design pattern for well controll the system.
    Your program only need declare one Zoo instance, and Start/Stop by default.

    Parameter
    --------
       args: The user control args, default user mpi/async training.
    """
    def __init__(self, args = '-net_type=mpi -sync=false'):
        """Start the Zoo, all the Actors will be registered.
        """
        check_call(LIB.HPPS_ZooStart(c_str(args)))

    def __del__(self):
        """Stop the Zoo, all the Actors will be destroyed
        """
        check_call(LIB.HPPS_ZooStop())

    def barrier(self):
        """Barrier all the nodes"""
        check_call(LIB.HPPS_ZooBarrier())

    def rank(self):
        """


        :returns: The rank of node

        """
        rank = ctypes.c_int()
        check_call(LIB.HPPS_ZooRank(ctypes.byref(rank)))
        return rank.value

    def size(self):
        """


        :returns: The size of nodes

        """
        size = ctypes.c_int()
        check_call(LIB.HPPS_ZooSize(ctypes.byref(size)))
        return size.value
    
    def num_workers(self):
        """


        :returns: the number of workers

        """
        num_workers = ctypes.c_int()
        check_call(LIB.HPPS_ZooNumWorkers(ctypes.byref(num_workers)))
        return num_workers.value

    def num_servers(self):
        """


        :returns: the number of servers

        """
        num_servers = ctypes.c_int()
        check_call(LIB.HPPS_ZooNumServers(ctypes.byref(num_servers)))
        return num_servers.value

    def is_worker(self):
        """

 
        :returns: whether the node is worker role.

        """
        is_worker = ctypes.c_int()
        check_call(LIB.HPPS_ZooIsWorker(ctypes.byref(is_worker)))
        return True if is_worker else False

    def is_server(self):
        """


        :returns: whether the node is server role.

        """
        is_server = ctypes.c_int()
        check_call(LIB.HPPS_ZooIsServer(ctypes.byref(is_server)))
        return True if is_server else False

    def is_controller(self):
        """


        :returns: whether the node is controller role.

        """
        is_controller = ctypes.c_int()
        check_call(LIB.HPPS_ZooIsController(ctypes.byref(is_controller)))
        return True if is_controller else False

    def worker_id(self):
        """


        :returns: the worker id [0, num_workers())

        """
        worker_id = ctypes.c_int()
        check_call(LIB.HPPS_ZooWorkerId(ctypes.byref(worker_id)))
        return worker_id.value

    def server_id(self):
        """


        :returns: the server id [0, num_servers())

        """
        server_id = ctypes.c_int()
        check_call(LIB.HPPS_ZooServerId(ctypes.byref(server_id)))
        return server_id.value

    def set_log_level(self, log_level):
        """

        Parameters:
        -----------

        log_level: 0 -> Debug 
                   1 -> Info
                   2 -> Error
                   3 -> Fatal
        """
        check_call(LIB.HPPS_ZooSetLogLevel(log_level))

    def set_log_file(self, log_file):
        """

        Parameters
        ----------

        log_file: The log file path for HPPS.

        """
        check_call(LIB.HPPS_ZooSetLogFile(c_str(log_file)))

