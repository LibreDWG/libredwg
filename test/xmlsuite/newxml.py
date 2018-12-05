import re
import sys
import os
def processData(line):
	re.compile(r"\w{3}\s\w{3}\s\d+\s\d{1,2}:\d{1,2}:\d{1,2}\s\d{4}")


DwgTxtFileName = sys.argv[1]
OutDir = sys.argv[2]
if not os.path.exists(DwgTxtFileName):
	sys.exit("File not found " + DwgTxtFileName)

Extension = DwgTxtFileName.split('.')
if Extension[-1] == 'txt':
	XMLFileName = OutDir + "/" + DwgTxtFileName.rstrip('txt') + "xml"
else:
	XMLFileName = OutDir + "/" + DwgTxtFileName+"xml"

FR = open(DwgTxtFileName,'r')

for line in FR:
	processData(line)
