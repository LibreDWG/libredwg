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

static PyMethodDef libredwg_methods[] = {
	    { "load", (PyCFunction)load_dwg, METH_VARARGS, NULL },
	    { NULL, NULL, 0, NULL }
};

PyMODINIT_FUNC initlibredwg()
{
	    Py_InitModule3("libredwg", libredwg_methods, "Module for handling DWG files.");
}

