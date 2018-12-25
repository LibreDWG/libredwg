# -*- indent-tabs-mode:1 tab-width:4 mode:python minor-mode:whitespace -*-
import lxml
import re
import os
import glob


'''
This class has all the colors to be used with colored output
terminal.
'''
class bcolors:
	HEADER = '\033[95m'
	OKBLUE = '\033[94m'
	OKGREEN = '\033[92m'
	WARNING = '\033[93m'
	FAIL = '\033[91m'
	ENDC = '\033[0m'


'''
This functions uses the script to generate xml which can be used for
comparison later.

@param string dwgdir The path to DWG dir
'''
def generatexml(dwgdir):
	# This beats ‘sys.argv[0]’, which is not guaranteed to be set.
	me = os.getenv ("PYTHON")
	if not me:
		me = "python"
	srcdir = os.path.dirname(__file__)
	current_dir = os.getcwd()
	os.chdir(dwgdir)
	for filename in glob.glob ("*/*.txt"):
		# maybe add double-quotes for the script?
		os.system (me + " " + srcdir + "/txttoxml.py " + filename + " "
					  + current_dir + "/test_output")
	os.chdir(current_dir)

'''
This functions main aim is to process special types of attributes
which are difficult to equate to each other. Currently this only
handles 2D and 3D point. It converts these string in certain format
so that they can be equated

@param string attr the attribute to be processed
@return string The processed attribute
'''
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
			return "(%.2f %.2f %.2f)" % (round(result[0],2), round(result[1],2),
										 round(result[2],2))
		elif len(result) == 2:
			return "(%.2f %.2f)" % round(round(result[0],2), round(result[1],2))
	else:
		return attr

'''
This function takes handle to both ideal file which came from AutoCAD and
practical file which came from LibreDWG and compares them to emit out the
result

@param ideal Name of the ideal file
@param practical Name of the practical file

return array[2]
[0] = The percentage of entity that matched
[1] = The unmatched attributes with following format
	{attrname, original, duplicate}
	attrname =  Name of the attribute
	original = Value came from AutoCAD
	duplicate = Value that came from LibreDWG.
'''
def xmlprocess(ideal, practical):
	doc = etree.parse(ideal)

	root = doc.getroot()
	child = root.getchildren

	# Let's first collect all the entities present in the file
	original_entities = []

	while child is not None:
		if child.type == "element":
			original_entities.insert(len(original_entities), child)
		child = child.next

	doc2 = etree.parse(practical)

	root2 = doc2.getroot()
	child2 = root2.getchildren

	duplicate_entities = []

	while child2 is not None:
		if child2.type == "element":
			duplicate_entities.insert(len(duplicate_entities), child2)
		child2 = child2.next

	match = 0

	# Now its time for comparison, For each dwg entity
	for original, duplicate in zip(original_entities, duplicate_entities):
		original_attributes = {}
		duplicate_attributes = {}
		excluded_attributes = ["Delta", "id", "Document", "Visible", "text",
							   "Application", "Hyperlinks"]

		# collect original attributes. Removing the attributes here, so the
		# total length is also set
		try:
			#print (ideal + " original.properties")
			for attr in original.properties:
				if attr.name not in excluded_attributes:
					original_attributes[attr.name] = processattr(attr.content)
		except (TypeError):
			print ("Need python3 compatible libxml2 with __next__ iterator")

		try:
			for attr in duplicate.properties:
				duplicate_attributes[attr.name] = processattr(attr.content)
		except (TypeError):
			pass

		unmatched_attr = []
		# collect duplicate attributes and check if it matches with
		# original ones
		for key,value in original_attributes.items():
			try:
				if value == duplicate_attributes[key]:
					match += 1
				else:
					# The attributes didn't match.
					# Report the unmatched attribute
					unmatched_attr.append({"attrname" : key, "original" : value,
									"duplicate" : duplicate_attributes[key]})

			except Exception:
				# This exception would occur when
				# We can't find the given attribute

				unmatched_attr.append({"attrname" : key, "original" : value,
									 "duplicate" : ""})
				continue

	# What are the total number of attributes
	try:
		total_attr = len(original_attributes)
		if total_attr == 0:
			percent_each = 0
		else:
			percent_each = 100 / total_attr
	except NameError:
		return [0,[]]
		raise


	res_percent = percent_each*match;

	#doc.freeDoc()
	#doc2.freeDoc()

	return [res_percent, unmatched_attr]
