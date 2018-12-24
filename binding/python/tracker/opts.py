""" Command line options for job submission script
"""
import os
import argparse

def get_opts(args = None):
    """ Get options to lauch the job

    Returns
    -------
      args: ArgumentParser.Argument
         The arguments returned by the parser
    """
    parser = argparse.ArgumentParser(description='Job submission')
    parser.add_argument('--cluster', type=str,
                        choices = ['mpi'],
                        help = 'Cluster type of this submission')
    parser.add_argument('--num-workers', required=True, type=int,
                        help = 'Number ofworker process to be launched')
    parser.add_argument('--host-file', required=True, type=str,
                        help='The file contains the list of hostnames, need for ssh')
    parser.add_argument('command', nargs='+', help = 'Command to be launched')

    (args, unkown) = parser.parse_known_args(args)
    args.command += unkown

    if args.cluster is None:
        raise RuntimeError('--cluster is not specified, youcan also specify')
    return args

