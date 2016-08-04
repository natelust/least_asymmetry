#include<Python.h>
#include<numpy/arrayobject.h>
#include <stdio.h>
#include <stdlib.h>

struct module_state {
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif


//#define IND(a,i) *((double *)(a->data+i*a->strides[0]))
#define IND(a,i,j) *((double *)(a->data+i*a->strides[0]+j*a->strides[1]))

static PyObject *make_asym_func(PyObject *self, PyObject *args);

static PyObject *make_asym_func(PyObject *self, PyObject *args)
{
  PyArrayObject *data, *dis, *weights;
  int w_truth;

  if(!PyArg_ParseTuple(args,"OOOi",&data,&dis,&weights,&w_truth)){
    return NULL;
  }

  if (!PyArray_SAMESHAPE(data,dis)){
    PyErr_Format(PyExc_ValueError,
		 "Shape of data array not equal to that of distance");
    return NULL;
  }
  
  int dis_i,dis_j,dis_ii,dis_jj,dis_dim0,dis_dim1;
  double r,weight_sum,sum_weight_mean,mu,core,asym,var;
  asym = 0;

  dis_dim0 = dis->dimensions[0];
  dis_dim1 = dis->dimensions[1];

  
  for(dis_i=0;dis_i<dis_dim0;dis_i++){
    for(dis_j=0;dis_j<dis_dim1;dis_j++){
      r = IND(dis,dis_i,dis_j);
      weight_sum = 0;
      sum_weight_mean = 0;
      core = 0;
      mu =0;
      var=0;

      for(dis_ii=0;dis_ii<dis_dim0;dis_ii++){
	for(dis_jj=0;dis_jj<dis_dim1;dis_jj++){
	  
	  if(IND(dis,dis_ii,dis_jj)==r){
	    weight_sum += IND(weights,dis_ii,dis_jj);
	    sum_weight_mean += IND(weights,dis_ii,dis_jj)*
	      IND(data,dis_ii,dis_jj);
	  }
	}
      }
      mu = sum_weight_mean/weight_sum;
      for(dis_ii=0;dis_ii<dis_dim0;dis_ii++){
	for(dis_jj=0;dis_jj<dis_dim1;dis_jj++){

	  if(IND(dis,dis_ii,dis_jj)==r){
	    core += IND(weights,dis_ii,dis_jj)*(IND(data,dis_ii,dis_jj)-mu)
	      *(IND(data,dis_ii,dis_jj)-mu);
	      }
	}
      }
      var = core/weight_sum;
      if(w_truth == 1){
	var = var*var;
      }
      asym += var;
	}
  }
  return Py_BuildValue("d",asym);
}


static PyMethodDef make_asym_methods[] = {
  {"make_asym_func",make_asym_func,METH_VARARGS},{NULL}};

#if PY_MAJOR_VERSION >= 3

static int make_asym_traverse(PyObject *m, visitproc visit, void *arg) {
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int make_asym_clear(PyObject *m) {
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}


static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "make_asym",
        NULL,
        sizeof(struct module_state),
        make_asym_methods,
        NULL,
        make_asym_traverse,
        make_asym_clear,
        NULL
};

#define INITERROR return NULL


//PyObject *

PyMODINIT_FUNC
PyInit_make_asym(void)
{
  import_array();
  return PyModule_Create(&moduledef);
}

#else
#define INITERROR return

#if PY_MAJOR_VERSION >= 3
#define NUMPY_IMPORT_ARRAY_RETURN_TYPE int
#else
#define NUMPY_IMPORT_ARRAY_RETURN_TYPE void

#endif

NUMPY_IMPORT_ARRAY_RETURN_TYPE
initmake_asym(void)
{
  import_array();
  return Py_InitModule("make_asym", make_asym_methods);
}
#endif

//int initnumpy()
//{
//  import_array();
//}
/*
{

#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&moduledef);
    import_array();
#else
    PyObject *module = Py_InitModule("make_asym", make_asym_methods);
    import_array();
#endif

    if (module == NULL)
        INITERROR;
    struct module_state *st = GETSTATE(module);

    st->error = PyErr_NewException("make_asym.Error", NULL, NULL);
    if (st->error == NULL) {
        Py_DECREF(module);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}
*/


//void intmake_asy() //for python2
//void initmake_asym()
//{
//  Py_InitModule("make_asym",make_asym_methods);
//  initnumpy();
//}

