#!/usr/bin/env python
from __future__ import absolute_import

import logging
import sys
import os

from . import opts
from . import mpi

def main():
    """ Main submission function
    """
    args = opts.get_opts()
    logging.basicConfig(format='%(asctime)s %(levelname)s %(message)s', level=logging.INFO)

    if args.cluster == 'mpi':
        mpi.submit(args)
    else:
        raise RuntimeError('Unkown submission cluster type %s' % args.cluster)

curr_path = os.path.dirname(os.path.abspath(os.path.expanduser(__file__)))
sys.path.insert(0, curr_path)

main()
