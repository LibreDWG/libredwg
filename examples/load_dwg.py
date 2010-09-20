#! /usr/bin/python

import libredwg
from libredwg import *

import sys

if (len(sys.argv) != 2):
        print "Usage: read_num_layers.py <filename>"
        exit()

filename = sys.argv[1]
a = Dwg_Data()
error = dwg_read_file(filename, a)

if (error != 0):
    print "Error!"
    exit()

print "Num objects: %d " % a.num_objects
print "Num layers: %d" % a.layer_control.tio.object.tio.LAYER_CONTROL.num_entries
