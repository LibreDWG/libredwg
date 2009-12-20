#include <Python.h>
#include <dwg.h>

Dwg_Data dwg;

static PyObject *load_dwg(PyObject *self, PyObject *args)
{
	char* filename;

	if (!PyArg_ParseTuple(args, "s", &filename))
	{
		return NULL;
	}

  /* Read dwg data */
  int success = dwg_read_file(filename, &dwg);
  if (success != 0)
    {
    	return Py_BuildValue("i", success);
    }

	return Py_BuildValue("i", 0);
}


static PyObject *get_version(PyObject *self, PyObject *args)
{
	char* version_string[]  = {"Older than R13", "R13", "R14", "R2000", "R2004", "R2007", "Newer than R2007"};
	return Py_BuildValue("s", version_string[dwg.header.version]);
}

static PyMethodDef libredwg_methods[] = {
	    { "load", (PyCFunction)load_dwg, METH_VARARGS, "Loads a DWG file into memory." },
	    { "get_version", (PyCFunction)get_version, METH_VARARGS, "Returns a string representing the version of the last opened DWG file." },
	    { NULL, NULL, 0, NULL }
};

PyMODINIT_FUNC initlibredwg()
{
	    Py_InitModule3("libredwg", libredwg_methods, "Module for handling DWG files.");
}

