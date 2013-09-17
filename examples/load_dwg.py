#! /usr/bin/python

import libredwg
from libredwg import *

import sys

if (len(sys.argv) != 2):
        print "Usage: read_num_layers.py <filename>"
        exit()

filename = sys.argv[1]
a = Dwg_Data()
a.object = new_Dwg_Object_Array(1000)
error = dwg_read_file(filename, a)

if (error != 0):
    print "Error!"
    exit()

print ".dwg version: %s" % a.header.version
print "Num objects: %d " % a.num_objects
#XXX for some reason I get a segfault when I try to read the LAYER_CONTROL in python (not in C)
#print "Num layers: %d" % a.layer_control.tio.object.tio.LAYER_CONTROL.num_entries

for i in range(0,a.num_objects):
    #XXX ugly, but works
    obj = Dwg_Object_Array_getitem(a.object,i)
    print " Supertype: " ,   obj.supertype
    print "      Type: " ,   obj.type
