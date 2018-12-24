""" MPI job submission
"""
from __future__ import absolute_import

import os, subprocess, logging
from threading import Thread

def sync_dir(local_dir, slave_node, slave_dir):
    """ Sync the working directory from root node to slave node
    """
    remote = slave_node + ":" + slave_dir
    logging.info('rsync %s -> %s', local_dir, remote)
    prog = 'rsync -az --rsh="ssh -o StrictHostKeyChecking=no -p %s" %s %s' % (22, local_dir, remote)
    mkdir = 'ssh %s "mkdir -p %s"' % (slave_node, slave_dir)
    subprocess.check_call([mkdir], shell = True)
    subprocess.check_call([prog], shell = True)
 
def get_hosts(host_file):
    """ Get host from MPI host file
    """
    fh = open(host_file)
    hosts = []
    for line in fh.readlines():
        splits = line.split(' ')
        hosts.append(splits[0])
    return hosts

def submit(args):
    """ Submission script with MPI
    """
    def mpi_submit(nworker):
        if args.host_file is not None:
            cmd = ' -hostfile %s' % (args.host_file)
        else:
            raise RuntimeError('--host-file can not none')
        cmd += ' ' + ' '.join(args.command)

        # sync program is necessary
        local_dir = os.getcwd() + '/'
        hosts = get_hosts(args.host_file)
        for h in hosts:
            sync_dir(local_dir, h, local_dir)

        # start workers
        if nworker > 0:
            prog = 'mpirun -np %d %s' % (nworker, cmd)
            logging.info('Start %d workers by mpirun: %s' % (nworker, prog))
            subprocess.check_call(prog, shell = True)
        else:
            raise RuntimeError('--num-workers must > 0')

    mpi_submit(args.num_workers)
