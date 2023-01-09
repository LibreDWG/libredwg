#!/usr/bin/env python
# -*- indent-tabs-mode:1 tab-width:4 mode:python minor-mode:whitespace -*-
import os
import sys

# Divide the scripts into two files
srcdir = os.path.dirname(__file__)
sys.path.append(os.getcwd())
# sys.path.append(srcdir)
from helper import *

# path where DWG files are present
path_to_dwg = srcdir + "/../test-data"
# The name of the output files folder
outdir = "test_output"

# Get all the directories
dirs = [d for d in os.listdir(path_to_dwg)
	if os.path.isdir(os.path.join(path_to_dwg, d)) and
		(d[0] == 'r' or (d.isdigit and int(d) >= 2000 and int(d) <= 2021))]

for dir in dirs:
	for file in os.listdir(os.path.join(path_to_dwg, dir)):
		if file.endswith(".dwg"):
			# First thing will be to make duplicate directory structure
			if not os.path.exists(outdir + "/" + dir):
				os.makedirs(outdir + "/" + dir)
			pass

# generate xml from txt files
generatexml(path_to_dwg)

# Now execute testsuite.c on all the DWG files found and create a separate directory structure
for dir in dirs:
	for file in os.listdir(os.path.join(path_to_dwg, dir)):
		if file.endswith(".dwg"):
			# filename of the XML File
			dwg_xmlfile = file.rsplit(".", 1)[0] + ".xml"
			if os.path.exists(path_to_dwg + "/" + dir + "/" + dwg_xmlfile):
				# Start running testsuite on every DWG file
				os.system("./testsuite " + path_to_dwg + "/"
						  + dir + "/" + file + " " + outdir
						  + "/" + dir + "/" + dwg_xmlfile + " 2> /dev/null")
		pass

# Now we have XML file. Next Up is Comparison
final_output = []
for dir in dirs:
	for file in os.listdir(os.path.join(path_to_dwg, dir)):
		if file.endswith(".xml"):

			# Duplicate file has same directory structure
			if os.path.exists(outdir + "/" + dir + "/" + file):
				result = xmlprocess(path_to_dwg+ "/" + dir + "/" + file,
									outdir + "/" + dir + "/" + file)
			else:
				result = [0, []]

			final_output.insert(len(final_output),
								[dir, file,result[0], result[1]])


# Now Generate a pretty report for it

#read the header
header = open(srcdir + "/header.htm","r")
reporthtm = open("result.htm", "w")
reporthtm.write(header.read())
current_format = ""
for report in final_output:
	if current_format != report[0]:
		# Print the header of the File Format
		reporthtm.write(
			"\n<div class='heading'>\n<h3>Output for %s File Format</h3>\n</div>"
			% report[0])
		print (bcolors.HEADER + "\n\n****Output for %s File Format****"
			   % report[0] + bcolors.ENDC)

	if report[2] < 100 and report[2] != 0:
		print (bcolors.OKGREEN + "%s: [%d]"
			   % (report[1], report[2]) + bcolors.ENDC)
		reporthtm.write("\n<div class='result_middle'><b>%s</b> matched <b>%d</b>%%</div>\n"
						% (report[1], report[2]))
	elif report[2] == 0:
		print (bcolors.WARNING + "%s: [%d]"
			   % (report[1], report[2]) + bcolors.ENDC)
		reporthtm.write("\n<div class='result_bad'>%s was not read at all</div>\n"
						% report[1])
	reporthtm.write("\n<div class='attributedetail'><h3>Attribute Details</h3>\n")
	for unmatched in report[3]:
		if unmatched['duplicate'] == "":
			reporthtm.write("\n<p><b>%s</b> wasn't found at all. Its value should be <b>%s</b></p>\n"
							% (unmatched['attrname'], unmatched['original']))
		else:
			reporthtm.write("\n<p><b>%s</b> didn't match. Its value should be <b>%s</b>, and it is <b>%s</b></p>\n"
							%(unmatched['attrname'],
							  unmatched["original"], unmatched['duplicate']))
	reporthtm.write("</div>")
	current_format = report[0]

# All information has been printed. Print the footer
print (bcolors.HEADER + "****End of Report****" + bcolors.ENDC)
reporthtm.write("<div><h1>End of Report</h1></div></body></html>");
reporthtm.close()
