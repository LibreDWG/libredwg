#! /usr/bin/env python3

#import libredwg
from libredwg import *

import sys

if (len(sys.argv) != 2):
        print("Usage: load_dwg.py <filename>")
        exit()

filename = sys.argv[1]
a = Dwg_Data()
a.object = new_Dwg_Object_Array(1000)
error = dwg_read_file(filename, a)

if (error > 0): # critical errors
    print("Error: ", error)
    if (error > 127):
        exit()

print(".dwg version: %s" % a.header.version)
print("Num objects: %d " % a.num_objects)

#XXX TODO Error: Dwg_Object_LAYER_CONTROL object has no attribute 'tio'
#print "Num layers: %d" % a.layer_control.tio.object.tio.LAYER_CONTROL.num_entries

#XXX ugly, but works
for i in range(0, a.num_objects):
    obj = Dwg_Object_Array_getitem(a.object, i)
    print(" Supertype: " ,   obj.supertype)
    print("      Type: " ,   obj.type)
