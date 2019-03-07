#include <Python.h>
#include <omp.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/**
 * Image filtering and processing
 *

 * Authors:
 *  UG Marco Antônio Gomes
 *  UG Júlio César Machado Álvares
 *	PhD Laerte Mateus Rodrigues
 **/

/**
 * Macros for ndarray access to return PyObject
 ***/
#define GET_3D(objsrc,d1,d2,d3) PyObject_GetItem(objsrc, Py_BuildValue("(iii)",d1,d2,d3))
#define SET_3D(objsrc,d1,d2,d3,objval) PyObject_SetItem(objsrc, Py_BuildValue("(iii)",d1,d2,d3),objval)

/**
 * External modules
 */
static PyObject *imageio;

/**
 * Structure class definition
 **/
typedef struct {
    PyObject_HEAD
    char *filename; // File name
    PyObject *image; // Image instance
    unsigned long width; // Image's width
    unsigned long height; // Image's height
    unsigned int threads; // Number of threads
    unsigned char ***buffer; // Image's buffer
} harpyClass;

///////////////////////////////////////
///        METHOD'S SECTION         ///
///////////////////////////////////////

static PyObject *harpy_tp_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    PyObject *self = PyType_GenericNew(type, args, kwargs);

    return self;
}

/**
 * __init__ method
 **/
static int harpy_tp_init(PyObject *self, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = { "filename" };
    harpyClass *class = (harpyClass *)self->ob_type;
    PyObject *imread = PyObject_GetAttrString(imageio, "imread");
    PyObject *shape;
    unsigned long i,j,k;

    // Default parameter values
    class->threads = get_nprocs();

    // Validate parameter
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "s|i:__init__", kwlist, &class->filename, &class->threads)) {
        return -1;
    }

    // Get Image
    class->image = PyObject_CallFunctionObjArgs(imread,Py_BuildValue("s",class->filename),NULL);
    shape = PyObject_GetAttrString(class->image,"shape");
    class->width = PyLong_AsLong(PyTuple_GetItem(shape,0));
    class->height = PyLong_AsLong(PyTuple_GetItem(shape, 1));

    // Copy image
    class->buffer = (unsigned char ***)malloc(class->width*sizeof(unsigned char ***));

    for(i=0;i<class->width;i++) {
        class->buffer[i] = (unsigned char **)malloc(class->height*sizeof(unsigned char **));

        for(j=0;j<class->height;j++) {
            class->buffer[i][j] = (unsigned char *)malloc(3*sizeof(unsigned char *));

            for(k=0; k<3; k++) {
                class->buffer[i][j][k] = (unsigned char)PyLong_AsLong(GET_3D(class->image,i,j,k));
            }
        }
    }

    Py_INCREF(self);

    return 0;
}

/**
 * Invert filter method
 **/
static PyObject *harpy_invert(PyObject *self, PyObject *args) {
    harpyClass *class = ((harpyClass *)self->ob_type);
    unsigned long i,j,k;

    #pragma omp parallel for private(i,j,k)
    for(i=0;i<class->width;i++) {
        for(j=0;j<class->height;j++) {
            for(k=0;k<3;k++) {
                class->buffer[i][j][k] = 255 - class->buffer[i][j][k];
            }
        }
    }
   
    Py_RETURN_NONE;
}

static PyObject *harpy_brightness(PyObject *self, PyObject *args) {
    harpyClass *class = ((harpyClass*)self->ob_type);
    int scale = 0;
    unsigned long i,j,k;

    if(!PyArg_ParseTuple(args, "i", &scale)) {
        return NULL;
    }

    #pragma omp parallel for private(i,j,k)
    for(i=0;i<class->width;i++) {
        for(j=0;j<class->height;j++) {
            for(k=0;k<3;k++) {
                if(class->buffer[i][j][k]+scale < 255) class->buffer[i][j][k] =  class->buffer[i][j][k] + scale;
                else if(class->buffer[i][j][k]+scale < 0) class->buffer[i][j][k] =  1;
                else class->buffer[i][j][k] =  255;
            }
        }
    }
    Py_RETURN_NONE;
}

static PyObject *harpy_contrast(PyObject *self, PyObject *args) {
    harpyClass *class = ((harpyClass*)self->ob_type);
    float scale = 0;
    unsigned long i,j,k;

    if(!PyArg_ParseTuple(args, "f", &scale)) {
        return NULL;
    }

    #pragma omp parallel for private(i,j,k)
    for(i=0;i<class->width;i++) {
        for(j=0;j<class->height;j++) {
            for(k=0;k<3;k++) {
                if(class->buffer[i][j][k]*scale < 255) class->buffer[i][j][k] =  class->buffer[i][j][k] * scale;
                else if(class->buffer[i][j][k]*scale < 0) class->buffer[i][j][k] =  1;
                else class->buffer[i][j][k] =  255;
            }
        }
    }

    Py_RETURN_NONE;
}

static PyObject *harpy_grayscaleAverage(PyObject *self, PyObject *args) {
    harpyClass *class = ((harpyClass*)self->ob_type);
    int average = 0;
    unsigned long i,j,k;

    #pragma omp parallel for private(i,j,k)
    for(i=0;i<class->width;i++) {
        for(j=0;j<class->height;j++) {
            average = (class->buffer[i][j][0] + class->buffer[i][j][1] + class->buffer[i][j][2])/3;
            for(k=0;k<3;k++) {
                if(average < 255) class->buffer[i][j][k] =  average;
                else if(average < 0) class->buffer[i][j][k] =  1;
                else class->buffer[i][j][k] =  255;
                          
            }
        }
    }

    Py_RETURN_NONE;
}

static PyObject *harpy_threshold(PyObject *self, PyObject *args) {
    harpyClass *class = ((harpyClass*)self->ob_type);
    int scale = 0;
    unsigned long i,j,k;

    if(!PyArg_ParseTuple(args, "i", &scale)) {
        return NULL;
    }

    #pragma omp parallel for private(i,j,k)
    for(i=0;i<class->width;i++) {
        for(j=0;j<class->height;j++) {
            for(k=0;k<3;k++) {
                if(class->buffer[i][j][k] > scale) class->buffer[i][j][k] = 255;
                else class->buffer[i][j][k] = 1;
                
            }
        }
    }

    Py_RETURN_NONE;
}

static PyObject *harpy_saltpepper(PyObject *self, PyObject *args) {
    harpyClass *class = ((harpyClass*)self->ob_type);
    int qtd = 0;
    int x = 0;
    unsigned long i,j,k, scale;

    if(!PyArg_ParseTuple(args, "i", &qtd)) {
        return NULL;
    }

    #pragma omp parallel for private(x, i, j, k, scale)
    for(x = 0; x < qtd; x++){
        i = rand() % class->width;
        j = rand() % class->height;
        k = rand() % 3;
        scale = rand() % 255;
        class->buffer[i][j][k] = scale;
    }
    Py_RETURN_NONE;
}

static PyObject *harpy_convolution(PyObject *self, PyObject *args) {
    harpyClass *class = ((harpyClass*)self->ob_type);
   
   // harpyClass *result;
    //memcpy(result, class, sizeof(class));

    int peso = 0;
    static int kernel[3][3] = {
        {-1,-1,-1},
        {-1,8,-1},
        {-1,-1,-1}
    };
    int width, height;
    width = height = (sizeof(kernel[0])/sizeof(int));

    unsigned long i,j, lin, col;
    int centroit;

    
    for( lin = 0; lin < 3; lin++){
        for(col = 0; col < 3; col++){
            peso += kernel[lin][col];
        }
    }
    printf("Peso: %d\n", peso);

    // if(!PyArg_ParseTuple(args, "i", &scale)) {
    //     return NULL;
    // }

    // #pragma omp parallel for private(i,j)
    // for(i = 0; i < result->width; i++){
    //     for(j = 0; i < result->height; j++){
    //         result->buffer[i][j][0] = 1;
    //         result->buffer[i][j][1] = 1;
    //         result->buffer[i][j][2] = 1;
    //     }
    // }

    #pragma omp parallel for private(i,j, lin, col, centroit)
    for(i = 0; i < class->width - width; i++) { 
        for(j = 0; j < class->height - height; j++) {
            for(lin = 0; lin < 3; lin++){
                for(col = 0; col < 3; col ++){
                    centroit += class->buffer[i+lin][j+col][0] * kernel[lin][col];
                }
            }
            if(peso != 0) centroit /= peso;

            if(centroit > 255) centroit = 255;
            else if(centroit <= 0) centroit = 1;

            class->buffer[i][j][0] = (int)centroit;
            class->buffer[i][j][1] = (int)centroit;
            class->buffer[i][j][2] = (int)centroit;

            // result->buffer[i][j][0] = (int)centroit;
            // result->buffer[i][j][1] = (int)centroit;
            // result->buffer[i][j][2] = (int)centroit;
            centroit = 0;
        }
    }
   // memcpy(class, result, sizeof(result));

    Py_RETURN_NONE;
}


static PyObject *harpy_save(PyObject *self, PyObject *args) {
    harpyClass *class = ((harpyClass *)self->ob_type);
    PyObject *imwrite = PyObject_GetAttrString(imageio, "imwrite");
    unsigned long i,j,k;
    char *filename = "temp.png";

    if(!PyArg_ParseTuple(args,"|s:save",&filename)) {
        return NULL;
    }
    
    for(i=0;i<class->width;i++) {
        for(j=0;j<class->height;j++) {
            for(k=0;k<3;k++) {
                SET_3D(class->image,i,j,k,Py_BuildValue("i",class->buffer[i][j][k]));
            }
        }
    }

    // Save Object
    PyObject_CallFunctionObjArgs(imwrite,Py_BuildValue("s",filename),class->image);

    Py_RETURN_NONE;
}

///////////////////////////////////////


/**
 * Module's methods
 **/
static PyMethodDef harpyModuleMethods[] = {
    {NULL, NULL, 0, NULL}
};

/**
 * Harpy Class methods
 **/
static PyMethodDef harpyClassMethods[] = {
    {"invert_filter",harpy_invert,METH_NOARGS,NULL},
    {"save",harpy_save,METH_VARARGS,NULL},
    {"brightness_filter", harpy_brightness,METH_VARARGS, NULL},
    {"contrast_filter", harpy_contrast, METH_VARARGS, NULL},
    {"grayscale_filter", harpy_grayscaleAverage, METH_NOARGS, NULL},
    {"threshold_filter", harpy_threshold, METH_VARARGS, NULL},
    {"saltandpepper_filter", harpy_saltpepper, METH_VARARGS, NULL},
    {"convolution_filter", harpy_convolution, METH_NOARGS, NULL}, //modficar para receber a matriz
    {NULL, NULL, 0, NULL}
};

/**
 * Class definition
 **/
static PyTypeObject harpyClassDef = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "harpy.Harpy",
    .tp_doc = "Harpy class for Image Filtering",
    .tp_basicsize = sizeof(harpyClass),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_init = harpy_tp_init,
    .tp_new = harpy_tp_new,
    .tp_methods = harpyClassMethods
};

/**
 * Module definition
 **/
static PyModuleDef harpyModuleDef = {
    PyModuleDef_HEAD_INIT,
    "harpy",
    "Harpy Module",
    -1,
    harpyModuleMethods
};

/**
 * Module Initializator
 **/
PyMODINIT_FUNC PyInit_harpy() {  
    PyObject *module = PyModule_Create(&harpyModuleDef);

    if(module == NULL) {
        return NULL;
    }

    if (PyType_Ready(&harpyClassDef) < 0) {
        return NULL;
    }
        
    // Modules import
    imageio = PyImport_ImportModule("imageio");

    // Class register
    Py_INCREF(&harpyClassDef);
    PyModule_AddObject(module, "Harpy", (PyObject *) &harpyClassDef);

    return module;
}
