#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#include <Python.h>
#include "numpy/arrayobject.h"

int main(int argc, char* argv[])
{
    setenv("PYTHONPATH", ".", 0);

    Py_Initialize();
    import_array();

    // Build the 2D array in C++
    const int SIZE = 3;
    npy_intp dims[2] {SIZE, SIZE};
    const int ND = 2;
    long double(*c_arr)[SIZE] { new long double[SIZE][SIZE] };

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            c_arr[i][j] = i + j;
        }
    }

    // Convert it to a NumPy array.
    PyObject* pArray = PyArray_SimpleNewFromData(ND, dims, NPY_LONGDOUBLE,
                       reinterpret_cast<void*>(c_arr));

    // import mymodule
    const char* module_name = "mymodule";
    PyObject* pName = PyUnicode_FromString(module_name);
    PyObject* pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    // import function
    const char* func_name = "array_tutorial";
    PyObject* pFunc = PyObject_GetAttrString(pModule, func_name);
    PyObject* pReturn = PyObject_CallFunctionObjArgs(pFunc, pArray, NULL);
    PyArrayObject* np_ret = reinterpret_cast<PyArrayObject*>(pReturn);

    // Convert back to C++ array and print.
    int len = PyArray_SHAPE(np_ret)[0];
    long double* c_out;
    c_out = reinterpret_cast<long double*>(PyArray_DATA(np_ret));
    std::cout << "Printing output array - C++" << std::endl;

    for (int i = 0; i < len; i++) {
        std::cout << c_out[i] << ' ';
    }

    std::cout << std::endl << std::endl;


    // import function without arguments
    const char* func_name2 = "myfunction";
    PyObject* pFunc2 = PyObject_GetAttrString(pModule, func_name2);
    PyObject* pReturn2 = PyObject_CallFunctionObjArgs(pFunc2, NULL);
    PyArrayObject* np_ret2 = reinterpret_cast<PyArrayObject*>(pReturn2);

    // convert back to C++ array and print
    int len2 = PyArray_SHAPE(np_ret2)[0];
    long double* c_out2;
    c_out2 = reinterpret_cast<long double*>(PyArray_DATA(np_ret2));
    std::cout << "Printing output array 2 - C++" << std::endl;

    for (int i = 0; i < len2; i++) {
        std::cout << c_out2[i] << ' ';
    }

    std::cout << std::endl << std::endl;

    Py_Finalize();
    return 0;
}
