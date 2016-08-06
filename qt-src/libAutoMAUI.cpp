//
// MATLAB Compiler: 6.0 (R2015a)
// Date: Thu Aug 04 17:34:35 2016
// Arguments: "-B" "macro_default" "-v" "-W" "cpplib:libAutoMAUI" "-T"
// "link:lib" "setup" "update" "autoInitializer" 
//

#include <stdio.h>
#define EXPORTING_libAutoMAUI 1
#include "libAutoMAUI.h"

static HMCRINSTANCE _mcr_inst = NULL;


#if defined( _MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__) || defined(__LCC__)
#ifdef __LCC__
#undef EXTERN_C
#endif
#include <windows.h>

static char path_to_dll[_MAX_PATH];

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, void *pv)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        if (GetModuleFileName(hInstance, path_to_dll, _MAX_PATH) == 0)
            return FALSE;
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
    }
    return TRUE;
}
#endif
#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultPrintHandler(const char *s)
{
  return mclWrite(1 /* stdout */, s, sizeof(char)*strlen(s));
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultErrorHandler(const char *s)
{
  int written = 0;
  size_t len = 0;
  len = strlen(s);
  written = mclWrite(2 /* stderr */, s, sizeof(char)*len);
  if (len > 0 && s[ len-1 ] != '\n')
    written += mclWrite(2 /* stderr */, "\n", sizeof(char));
  return written;
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_libAutoMAUI_C_API
#define LIB_libAutoMAUI_C_API /* No special import/export declaration */
#endif

LIB_libAutoMAUI_C_API 
bool MW_CALL_CONV libAutoMAUIInitializeWithHandlers(
    mclOutputHandlerFcn error_handler,
    mclOutputHandlerFcn print_handler)
{
    int bResult = 0;
  if (_mcr_inst != NULL)
    return true;
  if (!mclmcrInitialize())
    return false;
  if (!GetModuleFileName(GetModuleHandle("libAutoMAUI"), path_to_dll, _MAX_PATH))
    return false;
    {
        mclCtfStream ctfStream = 
            mclGetEmbeddedCtfStream(path_to_dll);
        if (ctfStream) {
            bResult = mclInitializeComponentInstanceEmbedded(   &_mcr_inst,
                                                                error_handler, 
                                                                print_handler,
                                                                ctfStream);
            mclDestroyStream(ctfStream);
        } else {
            bResult = 0;
        }
    }  
    if (!bResult)
    return false;
  return true;
}

LIB_libAutoMAUI_C_API 
bool MW_CALL_CONV libAutoMAUIInitialize(void)
{
  return libAutoMAUIInitializeWithHandlers(mclDefaultErrorHandler, 
                                           mclDefaultPrintHandler);
}

LIB_libAutoMAUI_C_API 
void MW_CALL_CONV libAutoMAUITerminate(void)
{
  if (_mcr_inst != NULL)
    mclTerminateInstance(&_mcr_inst);
}

LIB_libAutoMAUI_C_API 
void MW_CALL_CONV libAutoMAUIPrintStackTrace(void) 
{
  char** stackTrace;
  int stackDepth = mclGetStackTrace(&stackTrace);
  int i;
  for(i=0; i<stackDepth; i++)
  {
    mclWrite(2 /* stderr */, stackTrace[i], sizeof(char)*strlen(stackTrace[i]));
    mclWrite(2 /* stderr */, "\n", sizeof(char)*strlen("\n"));
  }
  mclFreeStackTrace(&stackTrace, stackDepth);
}


LIB_libAutoMAUI_C_API 
bool MW_CALL_CONV mlxSetup(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "setup", nlhs, plhs, nrhs, prhs);
}

LIB_libAutoMAUI_C_API 
bool MW_CALL_CONV mlxUpdate(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "update", nlhs, plhs, nrhs, prhs);
}

LIB_libAutoMAUI_C_API 
bool MW_CALL_CONV mlxAutoInitializer(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "autoInitializer", nlhs, plhs, nrhs, prhs);
}

LIB_libAutoMAUI_CPP_API 
void MW_CALL_CONV setup(int nargout, mwArray& smoothKernel, mwArray& derivateKernel, 
                        mwArray& topStrongLine, mwArray& botStrongLine, mwArray& 
                        topRefWall, mwArray& botRefWall, const mwArray& topStrongPoints, 
                        const mwArray& botStrongPoints)
{
  mclcppMlfFeval(_mcr_inst, "setup", nargout, 6, 2, &smoothKernel, &derivateKernel, &topStrongLine, &botStrongLine, &topRefWall, &botRefWall, &topStrongPoints, &botStrongPoints);
}

LIB_libAutoMAUI_CPP_API 
void MW_CALL_CONV update(int nargout, mwArray& topStrongLine, mwArray& botStrongLine, 
                         mwArray& OLD, mwArray& topWeakLine, mwArray& topIMT, mwArray& 
                         botWeakLine, mwArray& botIMT, mwArray& topWallRef, mwArray& 
                         botWallRef, const mwArray& frame, const mwArray& smoothKernel, 
                         const mwArray& derivateKernel, const mwArray& topStrongLine_in1, 
                         const mwArray& botStrongLine_in1, const mwArray& 
                         topStrongPoints, const mwArray& botStrongPoints, const mwArray& 
                         topWallRef_in1, const mwArray& botWallRef_in1)
{
  mclcppMlfFeval(_mcr_inst, "update", nargout, 9, 9, &topStrongLine, &botStrongLine, &OLD, &topWeakLine, &topIMT, &botWeakLine, &botIMT, &topWallRef, &botWallRef, &frame, &smoothKernel, &derivateKernel, &topStrongLine_in1, &botStrongLine_in1, &topStrongPoints, &botStrongPoints, &topWallRef_in1, &botWallRef_in1);
}

LIB_libAutoMAUI_CPP_API 
void MW_CALL_CONV autoInitializer(int nargout, mwArray& miniTopWall, mwArray& 
                                  miniBotWall, mwArray& kerUpHeight, mwArray& 
                                  kerBotHeight, const mwArray& ROI, const mwArray& 
                                  numPoints)
{
  mclcppMlfFeval(_mcr_inst, "autoInitializer", nargout, 4, 2, &miniTopWall, &miniBotWall, &kerUpHeight, &kerBotHeight, &ROI, &numPoints);
}

