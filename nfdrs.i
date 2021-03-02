// nfdrs.i

%module nfdrs
%{
#include "deadfuelmoisture.h"
%}
%{
#include "livefuelmoisture.h"
%}
%{
#include "DFMCalcState.h"
%}
%{
#include "LFMCalcState.h"
%}
%{
#include "NFDR2016CalcState.h"
%}
%{
#include "station.h"
%}
%{
#include "nfdrs2016.h"
%}

%include typemaps.i
%include "std_vector.i"
%include "std_string.i"
%include "std_iostream.i"

#ifdef SWIGPYTHON
typedef float FP_STORAGE_TYPE;
typedef long time_t;

%typemap(out) DoubleArray * {
  int len=$1->length, i;

  $result = PyList_New(len);

  for (i = 0; i < len; i++) {
    PyObject *o = PyFloat_FromDouble($1->arrayPtr[i]);
    PyList_SetItem($result,i,o);
  }
}
#endif

#ifdef SWIGPHP
%typemap(out) DoubleArray *{
   int len=$1->length, i;

   //while ($1[len])
   //     len++;

   array_init(return_value);

   for (i=0;i<len;i++){
        add_next_index_double(return_value, $1->arrayPtr[i]);
   }

   delete[] $1->arrayPtr;
   free($1);
}
#endif

%include "deadfuelmoisture.h"
%include "livefuelmoisture.h"
%include "DFMCalcState.h"
%include "LFMCalcState.h"
%include "NFDR2016CalcState.h"
%include "station.h"
%include "nfdrs2016.h
