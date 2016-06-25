//
// MATLAB Compiler: 4.14.1 (R2010bSP1)
// Date: Thu Jun 23 15:34:28 2016
// Arguments: "-B" "macro_default" "-v" "-W" "cpplib:autoInit" "-T" "link:lib"
// "autoInitializer" 
//

#ifndef __autoInit_h
#define __autoInit_h 1

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

#ifdef EXPORTING_autoInit
#define PUBLIC_autoInit_C_API __global
#else
#define PUBLIC_autoInit_C_API /* No import statement needed. */
#endif

#define LIB_autoInit_C_API PUBLIC_autoInit_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_autoInit
#define PUBLIC_autoInit_C_API __declspec(dllexport)
#else
#define PUBLIC_autoInit_C_API __declspec(dllimport)
#endif

#define LIB_autoInit_C_API PUBLIC_autoInit_C_API


#else

#define LIB_autoInit_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_autoInit_C_API 
#define LIB_autoInit_C_API /* No special import/export declaration */
#endif

extern LIB_autoInit_C_API 
bool MW_CALL_CONV autoInitInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_autoInit_C_API 
bool MW_CALL_CONV autoInitInitialize(void);

extern LIB_autoInit_C_API 
void MW_CALL_CONV autoInitTerminate(void);



extern LIB_autoInit_C_API 
void MW_CALL_CONV autoInitPrintStackTrace(void);

extern LIB_autoInit_C_API 
bool MW_CALL_CONV mlxAutoInitializer(int nlhs, mxArray *plhs[], int nrhs, mxArray 
                                     *prhs[]);

extern LIB_autoInit_C_API 
long MW_CALL_CONV autoInitGetMcrID();


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__BORLANDC__)

#ifdef EXPORTING_autoInit
#define PUBLIC_autoInit_CPP_API __declspec(dllexport)
#else
#define PUBLIC_autoInit_CPP_API __declspec(dllimport)
#endif

#define LIB_autoInit_CPP_API PUBLIC_autoInit_CPP_API

#else

#if !defined(LIB_autoInit_CPP_API)
#if defined(LIB_autoInit_C_API)
#define LIB_autoInit_CPP_API LIB_autoInit_C_API
#else
#define LIB_autoInit_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_autoInit_CPP_API void MW_CALL_CONV autoInitializer(int nargout, mwArray& miniTopWall, mwArray& miniBotWall, const mwArray& ROI, const mwArray& numPoints);

#endif
#endif
