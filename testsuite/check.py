#!/usr/bin/python
import os
import glob
import string
import sys
import libxml2
import re

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'

def processattr(attr):
	pattern = re.compile(r"(\d+\.\d{1,})\s{0,1}")
	if re.search(pattern, attr):
		#extract the numbers and adjust them
		extract_pattern = re.compile(r"(\d+\.\d{1,})\s{0,1}");
		result = extract_pattern.findall(attr)
		for no in range(len(result)):
			result[no] = float(result[no])

		#if its a 3d point
		if len(result) == 3:
			return "(%.2f %.2f %.2f)" % (round(result[0],2), round(result[1],2), round(result[2],2))
		elif len(result) == 2:
			return "(%.2f %.2f)" % round(round(result[0],2), round(result[1],2))
	else:
		return attr


def xmlprocess(ideal, practical):
	doc = libxml2.parseFile(ideal)

	root = doc.getRootElement()
	child = root.children

	original_entities = []

	while child is not None:
		if child.type == "element":
			original_entities.insert(len(original_entities), child)
		child = child.next

	doc2 = libxml2.parseFile(practical)

	root2 = doc2.getRootElement()
	child2 = root2.children

	duplicate_entities = []

	while child2 is not None:
		if child2.type == "element":
			duplicate_entities.insert(len(duplicate_entities), child2)
		child2 = child2.next

	match = 0

	#Now its time for comparison, For each Dwgenentity
	for original, duplicate in zip(original_entities, duplicate_entities):
		original_attributes = {}
		duplicate_attributes = {}

		#collect original attributes
		for attr in original.properties:
			original_attributes[attr.name] = processattr(attr.content)
		

		for attr in duplicate.properties:
			duplicate_attributes[attr.name] = processattr(attr.content)

		#Now just match the type attribute and leave the rest
		if original_attributes["type"] == duplicate_attributes["type"]:
			match = 1
		
		#collect duplicate attributes
		'''
		for attr in duplicate.properties:
			try:
				duplicate_attributes[attr.name] = processattr(attr.content);
				if original_attributes[attr.name] == duplicate_attributes[attr.name]:
					match+=1
				pass
			except Exception:
				# This exception would occur when
				# We can't find the given attribute
				continue
		

	#What are the total number of attributes
	try:
		total_attr = len(original_attributes)
		if total_attr == 0:
			percent_each = 0
		else:
			percent_each = 100 / total_attr
	except NameError:
		return 0
		raise


	res_percent = percent_each*match;
	'''

	doc.freeDoc()
	doc2.freeDoc()

	if match:
		return 100
	else:
		return 0

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
					break
				except OSError, e:
					if e.errno != 17:
						raise
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

