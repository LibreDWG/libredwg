# -*- indent-tabs-mode:1 tab-width:4 mode:python minor-mode:whitespace -*-
import os
import glob
import re
from lxml import etree


class bcolors:
    """This class has all the colors to be used with colored output terminal."""
    HEADER = "\033[95m"
    OKBLUE = "\033[94m"
    OKGREEN = "\033[92m"
    WARNING = "\033[93m"
    FAIL = "\033[91m"
    ENDC = "\033[0m"


def generatexml(dwgdir):
    """
    This function uses the script to generate xml which can be used for
    comparison later.

    @param str dwgdir: The path to DWG dir
    """
    me = os.getenv("PYTHON")
    if not me:
        me = "python"
    srcdir = os.path.dirname(__file__)
    current_dir = os.getcwd()
    os.chdir(dwgdir)
    for filename in glob.glob("*/*.txt"):
        os.system(
            me
            + " "
            + srcdir
            + "/txttoxml.py "
            + filename
            + " "
            + current_dir
            + "/test_output"
        )
    os.chdir(current_dir)


def processattr(attr):
    """
    This function's main aim is to process special types of attributes
    which are difficult to equate to each other. Currently this only
    handles 2D and 3D point. It converts these strings in certain format
    so that they can be equated.

    @param str attr: the attribute to be processed
    @return str: The processed attribute
    """
    pattern = re.compile(r"(\d+\.\d+)\s?")
    if re.search(pattern, attr):
        result = [float(x) for x in pattern.findall(attr)]

        if len(result) == 3:
            return "(%.2f %.2f %.2f)" % (
                round(result[0], 2),
                round(result[1], 2),
                round(result[2], 2),
            )
        elif len(result) == 2:
            return "(%.2f %.2f)" % (
                round(result[0], 2),
                round(result[1], 2)
            )
    return attr


def xmlprocess(ideal, practical):
    """
    This function takes handle to both ideal file which came from AutoCAD and
    practical file which came from LibreDWG and compares them to emit out the
    result.

    @param str ideal: Name of the ideal file
    @param str practical: Name of the practical file
    @return list: array[2]
        [0] = The percentage of entity that matched
        [1] = The unmatched attributes with following format
            {attrname, original, duplicate}
            attrname = Name of the attribute
            original = Value came from AutoCAD
            duplicate = Value that came from LibreDWG.
    """
    doc = etree.parse(ideal)
    root = doc.getroot()

    # Collect all entity elements (direct children of root)
    original_entities = [child for child in root if isinstance(child.tag, str)]

    doc2 = etree.parse(practical)
    root2 = doc2.getroot()

    # Collect all entity elements (direct children of root)
    duplicate_entities = [child for child in root2 if isinstance(child.tag, str)]

    match = 0
    total_unmatched = []

    # Now its time for comparison, For each dwg entity
    for original, duplicate in zip(original_entities, duplicate_entities):
        original_attributes = {}
        duplicate_attributes = {}
        excluded_attributes = [
            "Delta",
            "id",
            "Document",
            "Visible",
            "text",
            "Application",
            "Hyperlinks",
        ]

        # Collect original attributes
        try:
            for attr_name, attr_value in original.attrib.items():
                if attr_name not in excluded_attributes:
                    original_attributes[attr_name] = processattr(attr_value)
        except AttributeError:
            print("Need python3 compatible libxml2 with __next__ iterator")

        # Collect duplicate attributes
        try:
            for attr_name, attr_value in duplicate.attrib.items():
                if attr_name not in excluded_attributes:
                    duplicate_attributes[attr_name] = processattr(attr_value)
        except AttributeError:
            pass

        unmatched_attr = []

        # Compare attributes
        for key, value in original_attributes.items():
            try:
                if value == duplicate_attributes[key]:
                    match += 1
                else:
                    unmatched_attr.append(
                        {
                            "attrname": key,
                            "original": value,
                            "duplicate": duplicate_attributes[key],
                        }
                    )
            except KeyError:
                # Attribute not found in duplicate
                unmatched_attr.append(
                    {"attrname": key, "original": value, "duplicate": ""}
                )

        total_unmatched.extend(unmatched_attr)

    # Calculate percentage
    try:
        total_attr = len(original_attributes)
        if total_attr == 0:
            return [0, []]
        percent_each = 100 / total_attr
    except NameError:
        return [0, []]

    res_percent = percent_each * match

    return [res_percent, total_unmatched]
