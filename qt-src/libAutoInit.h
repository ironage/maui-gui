//
// MATLAB Compiler: 6.0 (R2015a)
// Date: Wed Jul 06 05:50:50 2016
// Arguments: "-B" "macro_default" "-v" "-W" "cpplib:libAutoInit" "-T"
// "link:lib" "autoInitializer" 
//

#ifndef __libAutoInit_h
#define __libAutoInit_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#include "mclcppclass.h"
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__SUNPRO_CC)
/* Solaris shared libraries use __global, rather than mapfiles
 * to define the API exported from a shared library. __global is
 * only necessary when building the library -- files including
 * this header file to use the library do not need the __global
 * declaration; hence the EXPORTING_<library> logic.
 */

#ifdef EXPORTING_libAutoInit
#define PUBLIC_libAutoInit_C_API __global
#else
#define PUBLIC_libAutoInit_C_API /* No import statement needed. */
#endif

#define LIB_libAutoInit_C_API PUBLIC_libAutoInit_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_libAutoInit
#define PUBLIC_libAutoInit_C_API __declspec(dllexport)
#else
#define PUBLIC_libAutoInit_C_API __declspec(dllimport)
#endif

#define LIB_libAutoInit_C_API PUBLIC_libAutoInit_C_API


#else

#define LIB_libAutoInit_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_libAutoInit_C_API 
#define LIB_libAutoInit_C_API /* No special import/export declaration */
#endif

extern LIB_libAutoInit_C_API 
bool MW_CALL_CONV libAutoInitInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_libAutoInit_C_API 
bool MW_CALL_CONV libAutoInitInitialize(void);

extern LIB_libAutoInit_C_API 
void MW_CALL_CONV libAutoInitTerminate(void);



extern LIB_libAutoInit_C_API 
void MW_CALL_CONV libAutoInitPrintStackTrace(void);

extern LIB_libAutoInit_C_API 
bool MW_CALL_CONV mlxAutoInitializer(int nlhs, mxArray *plhs[], int nrhs, mxArray 
                                     *prhs[]);


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__BORLANDC__)

#ifdef EXPORTING_libAutoInit
#define PUBLIC_libAutoInit_CPP_API __declspec(dllexport)
#else
#define PUBLIC_libAutoInit_CPP_API __declspec(dllimport)
#endif

#define LIB_libAutoInit_CPP_API PUBLIC_libAutoInit_CPP_API

#else

#if !defined(LIB_libAutoInit_CPP_API)
#if defined(LIB_libAutoInit_C_API)
#define LIB_libAutoInit_CPP_API LIB_libAutoInit_C_API
#else
#define LIB_libAutoInit_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_libAutoInit_CPP_API void MW_CALL_CONV autoInitializer(int nargout, mwArray& miniTopWall, mwArray& miniBotWall, const mwArray& ROI, const mwArray& numPoints);

#endif
#endif
