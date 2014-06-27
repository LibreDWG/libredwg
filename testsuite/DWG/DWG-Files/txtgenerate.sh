#!/bin/bash
for filename in */*.txt; do
	./txttoxml.py "$filename"
done
