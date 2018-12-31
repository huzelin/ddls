from __future__ import absolute_import

import os
import sys
import time
import types
import logging

def init_logging(log_level=0):
    ''' init logging
    '''
    fmt = '%(asctime)s [%(levelname)s] %(message)s'
    level = logging.INFO
    logging.basicConfig(format=fmt, level=level)

