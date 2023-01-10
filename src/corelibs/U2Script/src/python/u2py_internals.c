/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <Python.h>
#include <structmember.h>

#include <U2Script/U2Script.h>

#define BASE_NAME_LENGTH 50

const int FUNCTION_SUCCESS = 0;
const int FUNCTION_FAIL = -1;

/*
 * Check whether the `error` value is not U2_OK, and raise an appropriate exception
 *
 * Possible returning values:
 * FUNCTION_SUCCESS -       U2_OK == `error`
 * FUNCTION_FAIL -          otherwise
 *
 */

static int checkErrorCode(U2ErrorType error) {
    PyObject* exceptionObject = NULL;
    PyUnicodeObject* exceptionString = NULL;
    const wchar_t* errorString = NULL;
    Py_ssize_t errorStringLength = 0;
    if (U2_OK == error) {
        return FUNCTION_SUCCESS;
    }
    switch (error) {
        case U2_FAILED_TO_CREATE_FILE:
        case U2_FAILED_TO_CREATE_DIR:
        case U2_FAILED_TO_READ_FILE:
        case U2_FAILED_TO_REMOVE_TMP_FILE:
            exceptionObject = PyExc_IOError;
            break;
        case U2_INVALID_CALL:
            exceptionObject = PyExc_EnvironmentError;
            break;
        case U2_INVALID_NAME:
        case U2_INVALID_PATH:
        case U2_INVALID_STRING:
        case U2_NUM_ARG_OUT_OF_RANGE:
            exceptionObject = PyExc_ValueError;
            break;
        case U2_TOO_SMALL_BUFFER:
            exceptionObject = PyExc_BufferError;
            break;
        case U2_NOT_ENOUGH_MEMORY:
            exceptionObject = PyExc_MemoryError;
            break;
        default:
            exceptionObject = PyExc_Exception;
    }
    errorString = getErrorString(error);
    errorStringLength = (Py_ssize_t)wcslen(errorString);
    exceptionString = (PyUnicodeObject*)PyUnicode_FromWideChar(errorString, errorStringLength);
    PyErr_SetObject(exceptionObject, (PyObject*)exceptionString);
    return FUNCTION_FAIL;
}

/*
 * Release the memory acquired by the given C-string list.
 *
 * This function is supposed to be called only upon release of dynamically allocated array,
 * otherwise it causes memory violation error and program crash.
 *
 */

static void releaseStringList(int size, wchar_t** list) {
    int index = 0;
    for (; index < size; ++index) {
        free(list[index]);
    }
    free(list);
}

/*
 * Obtain last unsuccessful string value from UGENE environment
 * and build a Python object based on it
 *
 */

static PyObject* getLastFailedString() {
    const int initBufferSize = BASE_NAME_LENGTH * 2;
    int longNameSize = initBufferSize;
    wchar_t* longName = NULL;
    Py_ssize_t longNameLength = 0;
    U2ErrorType operationResult = U2_TOO_SMALL_BUFFER;
    PyObject* wrappedName = NULL;
    while (U2_TOO_SMALL_BUFFER == operationResult) {
        if (initBufferSize != longNameSize) {
            free(longName);
        }
        longName = (wchar_t*)malloc(longNameSize * sizeof(wchar_t));
        if (NULL == longName) {
            checkErrorCode(U2_NOT_ENOUGH_MEMORY);
            return NULL;
        }
        operationResult = getLastFailedStringValue(longNameSize, longName, &longNameSize);
    }
    longNameLength = (Py_ssize_t)wcslen(longName);
    wrappedName = PyUnicode_FromWideChar(longName, longNameLength);
    free(longName);
    return wrappedName;
}

/*
 * Wrap the `items` array of C-strings into a PyListObject instance
 *
 */

static PyListObject* convertToPyList(int itemsCount, const wchar_t** items) {
    PyListObject* outputFilesList = (PyListObject*)PyList_New(itemsCount);
    PyUnicodeObject* currentString = NULL;
    Py_ssize_t index = 0;

    for (; index < itemsCount; ++index) {
        const Py_ssize_t itemLength = (Py_ssize_t)wcslen(items[index]);
        currentString = (PyUnicodeObject*)PyUnicode_FromWideChar(items[index], itemLength);
        if (NULL == currentString) {
            checkErrorCode(U2_NOT_ENOUGH_MEMORY);
            return NULL;
        }
        PyList_SetItem((PyObject*)outputFilesList, index, (PyObject*)currentString);
    }
    return outputFilesList;
}

/*
 * Get Biopython's format name by U2Script's representation
 *
 */

static const char* getBioPythonFormatName(U2Format format) {
    switch (format) {
        case U2_ABI:
            return "abi";
        case U2_ACE:
            return "ace";
        case U2_CLUSTAL:
            return "clustal";
        case U2_EMBL:
            return "embl";
        case U2_FASTA:
            return "fasta";
        case U2_FASTQ:
            return "fastq";
        case U2_GENBANK:
            return "genbank";
        case U2_MEGA:
            return "mega";
        case U2_MSF:
            return "msf";
        case U2_NEXUS:
            return "nexus";
        case U2_STOCKHOLM:
            return "stockholm";
        case U2_SWISS:
            return "swiss";
        default:
            return "";
    }
}

/*
 * Computational scheme class implementation
 *
 */

typedef struct {
    PyObject_HEAD
        SchemeHandle handle;
} Scheme;

/*
 * Perform the memory allocation and basic initialization for a scheme object
 *
 */

static PyObject* Scheme_alloc(PyTypeObject* type, PyObject* args, PyObject* kwds) {
    Scheme* self = (Scheme*)type->tp_alloc(type, 0);
    kwds;
    if (NULL != self) {
        self->handle = NULL;
    }
    return (PyObject*)self;
}

/*
 * Initialize the internal scheme representation
 *
 */

static int Scheme_init(PyObject* self, PyObject* args, PyObject* kwds) {
    U2ErrorType result = U2_OK;
    const Py_UNICODE *pathToScheme = NULL, *inputFileForSas = NULL, *outputFileForSas = NULL;
    kwds;

    if (0 == PyArg_ParseTuple(args, "|uuu", &pathToScheme, &inputFileForSas, &outputFileForSas)) {
        return -1;
    }
    result = createSas((const wchar_t*)pathToScheme, (const wchar_t*)inputFileForSas, (const wchar_t*)outputFileForSas, &((Scheme*)self)->handle);
    if (U2_UNKNOWN_ELEMENT == result || U2_INVALID_STRING == result) {
        result = createScheme(pathToScheme, &((Scheme*)self)->handle);
    }
    if (FUNCTION_SUCCESS != checkErrorCode(result)) {
        return -1;
    }
    return 0;
}

/*
 * Deallocate all the resources acquired by the computational scheme
 *
 */

static void Scheme_dealloc(Scheme* self) {
    releaseScheme(self->handle);
    self->ob_type->tp_free((PyObject*)self);
}

/*
 * List of a Scheme instance fields
 *
 */

static PyMemberDef Scheme_members[] = {{NULL, 0, 0, 0, NULL}};

/*
 * Scheme class methods implementation
 *
 */

static PyObject* Scheme_addElement(PyObject* self, PyObject* args) {
    U2ErrorType result = U2_OK;
    const Py_UNICODE* elementTypeName = NULL;
    Py_UNICODE elementName[BASE_NAME_LENGTH];

    if (0 == PyArg_ParseTuple(args, "u", &elementTypeName)) {
        return NULL;
    }
    result = addElementToScheme(((Scheme*)self)->handle, (const wchar_t*)elementTypeName, BASE_NAME_LENGTH, (wchar_t*)elementName);

    if (U2_TOO_SMALL_BUFFER == result) {
        return getLastFailedString();
    } else if (FUNCTION_SUCCESS != checkErrorCode(result)) {
        return NULL;
    }
    return Py_BuildValue("u", elementName);
}

static PyObject* Scheme_addReader(PyObject* self, PyObject* args) {
    U2ErrorType result = U2_OK;
    const Py_UNICODE *readerTypeName = NULL, *inputFilePath = NULL;
    Py_UNICODE readerName[BASE_NAME_LENGTH];

    if (0 == PyArg_ParseTuple(args, "uu", &readerTypeName, &inputFilePath)) {
        return NULL;
    }
    result = addReaderToScheme(((Scheme*)self)->handle, (const wchar_t*)readerTypeName, (const wchar_t*)inputFilePath, BASE_NAME_LENGTH, (wchar_t*)readerName);

    if (U2_TOO_SMALL_BUFFER == result) {
        return getLastFailedString();
    } else if (FUNCTION_SUCCESS != checkErrorCode(result)) {
        return NULL;
    }
    return Py_BuildValue("u", readerName);
}

static PyObject* Scheme_addWriter(PyObject* self, PyObject* args) {
    U2ErrorType result = U2_OK;
    const Py_UNICODE *writerTypeName = NULL, *outputFilePath = NULL;
    Py_UNICODE writerName[BASE_NAME_LENGTH];

    if (0 == PyArg_ParseTuple(args, "uu", &writerTypeName, &outputFilePath)) {
        return NULL;
    }
    result = addWriterToScheme(((Scheme*)self)->handle, (const wchar_t*)writerTypeName, (const wchar_t*)outputFilePath, BASE_NAME_LENGTH, (wchar_t*)writerName);

    if (U2_TOO_SMALL_BUFFER == result) {
        return getLastFailedString();
    } else if (FUNCTION_SUCCESS != checkErrorCode(result)) {
        return NULL;
    }
    return Py_BuildValue("u", writerName);
}

static PyObject* Scheme_addActorsBinding(PyObject* self, PyObject* args) {
    U2ErrorType result = U2_OK;
    const Py_UNICODE *srcElementName = NULL, *srcSlotName = NULL, *dstElementName = NULL,
                     *dstPortAndSlotNames = NULL;

    if (0 == PyArg_ParseTuple(args, "uuuu", &srcElementName, &srcSlotName, &dstElementName, &dstPortAndSlotNames)) {
        return NULL;
    }
    result = addSchemeActorsBinding(((Scheme*)self)->handle,
                                    (const wchar_t*)srcElementName,
                                    (const wchar_t*)srcSlotName,
                                    (const wchar_t*)dstElementName,
                                    (const wchar_t*)dstPortAndSlotNames);

    if (FUNCTION_SUCCESS != checkErrorCode(result)) {
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject* Scheme_addFlow(PyObject* self, PyObject* args) {
    U2ErrorType result = U2_OK;
    const Py_UNICODE *srcElementName = NULL, *srcPortName = NULL, *dstElementName = NULL,
                     *dstPortName = NULL;

    if (0 == PyArg_ParseTuple(args, "uuuu", &srcElementName, &srcPortName, &dstElementName, &dstPortName)) {
        return NULL;
    }
    result = addFlowToScheme(((Scheme*)self)->handle, (const wchar_t*)srcElementName, (const wchar_t*)srcPortName, (const wchar_t*)dstElementName, (const wchar_t*)dstPortName);

    if (FUNCTION_SUCCESS != checkErrorCode(result)) {
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject* Scheme_setElementAttribute(PyObject* self, PyObject* args) {
    U2ErrorType result = U2_OK;
    const Py_UNICODE *elementName = NULL, *attributeName = NULL, *attributeValue = NULL;

    if (0 == PyArg_ParseTuple(args, "uuu", &elementName, &attributeName, &attributeValue)) {
        return NULL;
    }

    result = setSchemeElementAttribute(((Scheme*)self)->handle,
                                       (const wchar_t*)elementName,
                                       (const wchar_t*)attributeName,
                                       (const wchar_t*)attributeValue);
    if (FUNCTION_SUCCESS != checkErrorCode(result)) {
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject* Scheme_getElementAttribute(PyObject* self, PyObject* args) {
    U2ErrorType result = U2_OK;
    const Py_UNICODE *elementName = NULL, *attributeName = NULL;
    Py_UNICODE attributeValue[BASE_NAME_LENGTH];

    if (0 == PyArg_ParseTuple(args, "uu", &elementName, &attributeName)) {
        return NULL;
    }
    result = getSchemeElementAttribute(((Scheme*)self)->handle,
                                       (const wchar_t*)elementName,
                                       (const wchar_t*)attributeName,
                                       BASE_NAME_LENGTH,
                                       (wchar_t*)attributeValue);
    if (U2_TOO_SMALL_BUFFER == result) {
        return getLastFailedString();
    } else if (FUNCTION_SUCCESS != checkErrorCode(result)) {
        return NULL;
    }
    return Py_BuildValue("s", attributeValue);
}

static PyObject* Scheme_launch(Scheme* self) {
    PyListObject* outputFileList = NULL;
    wchar_t** outputFilePaths = NULL;
    int fileCount = 0;

    U2ErrorType result = launchScheme(self->handle, &fileCount, &outputFilePaths);
    if (FUNCTION_SUCCESS != checkErrorCode(result)) {
        return NULL;
    }
    outputFileList = convertToPyList(fileCount, outputFilePaths);
    releaseStringList(fileCount, outputFilePaths);
    return (PyObject*)outputFileList;
}

static PyObject* Scheme_launchSas(PyObject* self, PyObject* args) {
    PyListObject* outputFileList = NULL;
    wchar_t** outputFilePaths = NULL;
    int fileCount = 0;
    U2ErrorType result = U2_OK;
    const Py_UNICODE *algorithmName = NULL, *inputPath = NULL, *outputPath = NULL;

    if (0 == PyArg_ParseTuple(args, "uuu", &algorithmName, &inputPath, &outputPath)) {
        return NULL;
    }
    result = launchSas((const wchar_t*)algorithmName, (const wchar_t*)inputPath, (const wchar_t*)outputPath, &fileCount, &outputFilePaths);
    if (FUNCTION_SUCCESS != checkErrorCode(result)) {
        return NULL;
    }
    outputFileList = convertToPyList(fileCount, outputFilePaths);
    releaseStringList(fileCount, outputFilePaths);
    return (PyObject*)outputFileList;
}

static PyObject* Scheme_saveToFile(PyObject* self, PyObject* args) {
    U2ErrorType result = U2_OK;
    const Py_UNICODE* path = NULL;

    if (0 == PyArg_ParseTuple(args, "u", &path)) {
        return NULL;
    }
    result = saveSchemeToFile(((Scheme*)self)->handle, (const wchar_t*)path);
    if (FUNCTION_SUCCESS != checkErrorCode(result)) {
        return NULL;
    }
    Py_RETURN_NONE;
}

/*
 * List of the Scheme class methods
 *
 */

static PyMethodDef Scheme_methods[] = {
    {"addElement", (PyCFunction)Scheme_addElement, METH_VARARGS, "Add to the computational scheme a new element of the supplied type"
                                                                 "and get its name"},
    {"addReader", (PyCFunction)Scheme_addReader, METH_VARARGS, "Add to the computational scheme a new read element of the supplied type,"
                                                               "set its input file path and get the element's name"},
    {"addWriter", (PyCFunction)Scheme_addWriter, METH_VARARGS, "Add to the computational scheme a new write element of the supplied type,"
                                                               "set its output file path and get the element's name"},
    {"addFlow", (PyCFunction)Scheme_addFlow, METH_VARARGS, "Set binding between the given ports of the given computational elements."},
    {"addActorsBinding", (PyCFunction)Scheme_addActorsBinding, METH_VARARGS, "Set binding between the given data slots of the given computational elements."},
    {"setElementAttribute", (PyCFunction)Scheme_setElementAttribute, METH_VARARGS, "Specify the given attribute of the supplied element"},
    {"getElementAttribute", (PyCFunction)Scheme_getElementAttribute, METH_VARARGS, "Get the value of the given element's attribute"},
    {"launch", (PyCFunction)Scheme_launch, METH_NOARGS, "Run the scheme synchronously by the Workflow Designer"},
    {"saveToFile", (PyCFunction)Scheme_saveToFile, METH_VARARGS, "Save the computational scheme to file in the UWL format to the given path"},
    {"launchSas", (PyCFunction)Scheme_launchSas, METH_VARARGS | METH_STATIC, "Create and run the Single Algorithm Scheme synchronously by the Workflow Designer"},
    {NULL, NULL, 0, NULL}};

/*
 * Scheme type definition
 *
 */

static PyTypeObject SchemeType = {
    PyObject_HEAD_INIT(NULL) 0, /*  ob_size*/
    "u2py_internals.Scheme", /*  tp_name*/
    sizeof(Scheme), /*  tp_basicsize*/
    0, /*  tp_itemsize*/
    (destructor)Scheme_dealloc, /*  tp_dealloc*/
    0, /*  tp_print*/
    0, /*  tp_getattr*/
    0, /*  tp_setattr*/
    0, /*  tp_compare*/
    0, /*  tp_repr*/
    0, /*  tp_as_number*/
    0, /*  tp_as_sequence*/
    0, /*  tp_as_mapping*/
    0, /*  tp_hash */
    0, /*  tp_call*/
    0, /*  tp_str*/
    0, /*  tp_getattro*/
    0, /*  tp_setattro*/
    0, /*  tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*  tp_flags*/
    "Workflow Designer scheme objects", /*  tp_doc */
    0, /*  tp_traverse */
    0, /*  tp_clear */
    0, /*  tp_richcompare */
    0, /*  tp_weaklistoffset */
    0, /*  tp_iter */
    0, /*  tp_iternext */
    Scheme_methods, /*  tp_methods */
    Scheme_members, /*  tp_members */
    0, /*  tp_getset */
    0, /*  tp_base */
    0, /*  tp_dict */
    0, /*  tp_descr_get */
    0, /*  tp_descr_set */
    0, /*  tp_dictoffset */
    (initproc)Scheme_init, /*  tp_init */
    0, /*  tp_alloc */
    Scheme_alloc /*  tp_new */
};

static PyObject* u2py_internals_initContext(PyObject* self, PyObject* args) {
    const Py_UNICODE* workingDirectory = NULL;
    U2ErrorType result = U2_OK;

    if (0 == PyArg_ParseTuple(args, "u", &workingDirectory)) {
        return NULL;
    }
    result = initContext((const wchar_t*)workingDirectory);
    if (FUNCTION_SUCCESS != checkErrorCode(result)) {
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject* u2py_internals_detectFormat(PyObject* self, PyObject* args) {
    const Py_UNICODE* filePath = NULL;
    U2ErrorType result = U2_OK;
    U2Format format = U2_UNSUPPORTED;
    PyObject* formatName = NULL;

    if (0 == PyArg_ParseTuple(args, "u", &filePath)) {
        return NULL;
    }
    result = detectFileFormat((const wchar_t*)filePath, &format);
    if (FUNCTION_SUCCESS != checkErrorCode(result)) {
        return NULL;
    }
    return Py_BuildValue("s", getBioPythonFormatName(format));
}

static PyObject* u2py_internals_releaseContext(PyObject* self, PyObject* args) {
    U2ErrorType result = releaseContext();
    if (FUNCTION_SUCCESS != checkErrorCode(result)) {
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyMethodDef u2py_internals_methods[] = {
    {"initContext", u2py_internals_initContext, METH_VARARGS, "Initialize UGENE environment, create all plugins and services"},
    {"detectFormat", u2py_internals_detectFormat, METH_VARARGS, "Get BioPython's format name for the given file"},
    {"releaseContext", u2py_internals_releaseContext, METH_NOARGS, "Deallocate all resources acquired by UGENE environment"},
    {NULL, NULL, 0, NULL}};

/*
 * Initialize and register 'u2py_internals' module
 *
 */

PyMODINIT_FUNC initu2py_internals() {
    PyObject* mainModule = NULL;

    mainModule = Py_InitModule("u2py_internals", u2py_internals_methods);
    if (NULL == mainModule || 0 > PyType_Ready(&SchemeType)) {
        return;
    }
    Py_INCREF(&SchemeType);
    PyModule_AddObject(mainModule, "Scheme", (PyObject*)&SchemeType);
}
