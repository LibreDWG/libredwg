#!/usr/bin/python
import os
import glob
import string
import sys
import libxml2
import re

'''
Fix the first time error
Get the file with some usage information
generate html Output
Generate the time to execute with it if possible
show the attributes
'''

# Divide the scripts in to files
sys.path.append(os.getcwd())
from helper import *

path_to_dwg = "DWG/DWG-Files"
testoutput_folder = "test_output"

# Get all the directories
dirs = [d for d in os.listdir(path_to_dwg) if os.path.isdir(os.path.join(path_to_dwg, d))]

#Now execute testsuite.c on all the DWG files found and create a seperate directory structure
for dir in dirs:
	for file in os.listdir(os.path.join(path_to_dwg, dir)):
		if file.endswith(".dwg"):
			#First thing will be to make duplicate directory structure
			if not os.path.exists(testoutput_folder+"/"+dir):
				#Need to be careful here
				try:
					os.makedirs(testoutput_folder+"/"+dir)
				except OSError, e:
					pass
			pass


			#Decide the filename of the XML File
			dwg_xmlfile = file.rsplit(".", 1)[0]+".xml"

			#Start running testsuite on every DWG file
			#print "testsuite "+path_to_dwg+"/"+dir+"/"+file+" "+testoutput_folder+"/"+dir+"/"+dwg_xmlfile

			os.system("./testsuite "+path_to_dwg+"/"+dir+"/"+file+" "+testoutput_folder+"/"+dir+"/"+dwg_xmlfile + " 2> /dev/null")
		pass

#Now we have XML file. Next Up is Comparison
final_output = []
for dir in dirs:
	for file in os.listdir(os.path.join(path_to_dwg, dir)):
		if file.endswith(".xml"):

			#Duplicate file has same directory structure
			if os.path.exists(testoutput_folder+"/"+dir+"/"+file):
				file_percent = xmlprocess(path_to_dwg+"/"+dir+"/"+file, testoutput_folder+"/"+dir+"/"+file)
			else:
				file_percent = 0	
			final_output.insert(len(final_output), [dir, file, file_percent])


# Now Generate a pretty report for it
current_format = ""
for report in final_output:
	if current_format != report[0]:
		print bcolors.HEADER+"\n\n****Output for %s File Format****" % report[0]+ bcolors.ENDC

	if report[2] == 100:
		print bcolors.OKGREEN + "%s: [%d]" % (report[1], report[2]) + bcolors.ENDC
	else:
		print bcolors.WARNING + "%s: [%d]" % (report[1], report[2]) + bcolors.ENDC 
	current_format = report[0]

print bcolors.HEADER + "****End of Report****" + bcolors.ENDC

