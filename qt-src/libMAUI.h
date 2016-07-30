//
// MATLAB Compiler: 6.0 (R2015a)
// Date: Wed Jul 06 05:44:47 2016
// Arguments: "-B" "macro_default" "-v" "-W" "cpplib:libMAUI" "-T" "link:lib"
// "setup" "update" 
//

#ifndef __libMAUI_h
#define __libMAUI_h 1

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

#ifdef EXPORTING_libMAUI
#define PUBLIC_libMAUI_C_API __global
#else
#define PUBLIC_libMAUI_C_API /* No import statement needed. */
#endif

#define LIB_libMAUI_C_API PUBLIC_libMAUI_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_libMAUI
#define PUBLIC_libMAUI_C_API __declspec(dllexport)
#else
#define PUBLIC_libMAUI_C_API __declspec(dllimport)
#endif

#define LIB_libMAUI_C_API PUBLIC_libMAUI_C_API


#else

#define LIB_libMAUI_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_libMAUI_C_API 
#define LIB_libMAUI_C_API /* No special import/export declaration */
#endif

extern LIB_libMAUI_C_API 
bool MW_CALL_CONV libMAUIInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_libMAUI_C_API 
bool MW_CALL_CONV libMAUIInitialize(void);

extern LIB_libMAUI_C_API 
void MW_CALL_CONV libMAUITerminate(void);



extern LIB_libMAUI_C_API 
void MW_CALL_CONV libMAUIPrintStackTrace(void);

extern LIB_libMAUI_C_API 
bool MW_CALL_CONV mlxSetup(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_libMAUI_C_API 
bool MW_CALL_CONV mlxUpdate(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__BORLANDC__)

#ifdef EXPORTING_libMAUI
#define PUBLIC_libMAUI_CPP_API __declspec(dllexport)
#else
#define PUBLIC_libMAUI_CPP_API __declspec(dllimport)
#endif

#define LIB_libMAUI_CPP_API PUBLIC_libMAUI_CPP_API

#else

#if !defined(LIB_libMAUI_CPP_API)
#if defined(LIB_libMAUI_C_API)
#define LIB_libMAUI_CPP_API LIB_libMAUI_C_API
#else
#define LIB_libMAUI_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_libMAUI_CPP_API void MW_CALL_CONV setup(int nargout, mwArray& smoothKernel, mwArray& derivateKernel, mwArray& topStrongLine, mwArray& botStrongLine, mwArray& topRefWall, mwArray& botRefWall, const mwArray& topStrongPoints, const mwArray& botStrongPoints);

extern LIB_libMAUI_CPP_API void MW_CALL_CONV update(int nargout, mwArray& topStrongLine, mwArray& botStrongLine, mwArray& OLD, mwArray& topWeakLine, mwArray& topIMT, mwArray& botWeakLine, mwArray& botIMT, mwArray& topWallRef, mwArray& botWallRef, const mwArray& frame, const mwArray& smoothKernel, const mwArray& derivateKernel, const mwArray& topStrongLine_in1, const mwArray& botStrongLine_in1, const mwArray& topStrongPoints, const mwArray& botStrongPoints, const mwArray& topWallRef_in1, const mwArray& botWallRef_in1);

#endif
#endif