//
// MATLAB Compiler: 6.0 (R2015a)
// Date: Fri Jun 09 16:22:26 2017
// Arguments: "-B" "macro_default" "-v" "-W"
// "cpplib:MAUIVelocityDllWithAutoInit" "-T" "link:lib" "update" "setup"
// "autoInitializer" "setup4Velocity" "check4FirstMovingFrame"
// "processVelocityIntervals" 
//

#include <stdio.h>
#define EXPORTING_MAUIVelocityDllWithAutoInit 1
#include "MAUIVelocityDllWithAutoInit.h"

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
#ifndef LIB_MAUIVelocityDllWithAutoInit_C_API
#define LIB_MAUIVelocityDllWithAutoInit_C_API /* No special import/export declaration */
#endif

LIB_MAUIVelocityDllWithAutoInit_C_API 
bool MW_CALL_CONV MAUIVelocityDllWithAutoInitInitializeWithHandlers(
    mclOutputHandlerFcn error_handler,
    mclOutputHandlerFcn print_handler)
{
    int bResult = 0;
  if (_mcr_inst != NULL)
    return true;
  if (!mclmcrInitialize())
    return false;
  if (!GetModuleFileName(GetModuleHandle("MAUIVelocityDllWithAutoInit"), path_to_dll, _MAX_PATH))
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

LIB_MAUIVelocityDllWithAutoInit_C_API 
bool MW_CALL_CONV MAUIVelocityDllWithAutoInitInitialize(void)
{
  return MAUIVelocityDllWithAutoInitInitializeWithHandlers(mclDefaultErrorHandler, 
                                                           mclDefaultPrintHandler);
}

LIB_MAUIVelocityDllWithAutoInit_C_API 
void MW_CALL_CONV MAUIVelocityDllWithAutoInitTerminate(void)
{
  if (_mcr_inst != NULL)
    mclTerminateInstance(&_mcr_inst);
}

LIB_MAUIVelocityDllWithAutoInit_C_API 
void MW_CALL_CONV MAUIVelocityDllWithAutoInitPrintStackTrace(void) 
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


LIB_MAUIVelocityDllWithAutoInit_C_API 
bool MW_CALL_CONV mlxUpdate(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "update", nlhs, plhs, nrhs, prhs);
}

LIB_MAUIVelocityDllWithAutoInit_C_API 
bool MW_CALL_CONV mlxSetup(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "setup", nlhs, plhs, nrhs, prhs);
}

LIB_MAUIVelocityDllWithAutoInit_C_API 
bool MW_CALL_CONV mlxAutoInitializer(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "autoInitializer", nlhs, plhs, nrhs, prhs);
}

LIB_MAUIVelocityDllWithAutoInit_C_API 
bool MW_CALL_CONV mlxSetup4Velocity(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "setup4Velocity", nlhs, plhs, nrhs, prhs);
}

LIB_MAUIVelocityDllWithAutoInit_C_API 
bool MW_CALL_CONV mlxCheck4FirstMovingFrame(int nlhs, mxArray *plhs[], int nrhs, mxArray 
                                            *prhs[])
{
  return mclFeval(_mcr_inst, "check4FirstMovingFrame", nlhs, plhs, nrhs, prhs);
}

LIB_MAUIVelocityDllWithAutoInit_C_API 
bool MW_CALL_CONV mlxProcessVelocityIntervals(int nlhs, mxArray *plhs[], int nrhs, 
                                              mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "processVelocityIntervals", nlhs, plhs, nrhs, prhs);
}

LIB_MAUIVelocityDllWithAutoInit_CPP_API 
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

LIB_MAUIVelocityDllWithAutoInit_CPP_API 
void MW_CALL_CONV setup(int nargout, mwArray& smoothKernel, mwArray& derivateKernel, 
                        mwArray& topStrongLine, mwArray& botStrongLine, mwArray& 
                        topRefWall, mwArray& botRefWall, const mwArray& topStrongPoints, 
                        const mwArray& botStrongPoints)
{
  mclcppMlfFeval(_mcr_inst, "setup", nargout, 6, 2, &smoothKernel, &derivateKernel, &topStrongLine, &botStrongLine, &topRefWall, &botRefWall, &topStrongPoints, &botStrongPoints);
}

LIB_MAUIVelocityDllWithAutoInit_CPP_API 
void MW_CALL_CONV autoInitializer(int nargout, mwArray& miniTopWall, mwArray& 
                                  miniBotWall, mwArray& kerUpHeight, mwArray& 
                                  kerBotHeight, const mwArray& ROI, const mwArray& 
                                  numPoints)
{
  mclcppMlfFeval(_mcr_inst, "autoInitializer", nargout, 4, 2, &miniTopWall, &miniBotWall, &kerUpHeight, &kerBotHeight, &ROI, &numPoints);
}

LIB_MAUIVelocityDllWithAutoInit_CPP_API 
void MW_CALL_CONV setup4Velocity(int nargout, mwArray& xAxisLocation, mwArray& videoType, 
                                 const mwArray& currentVelocityFrame, const mwArray& 
                                 previousVelocityFrame)
{
  mclcppMlfFeval(_mcr_inst, "setup4Velocity", nargout, 2, 2, &xAxisLocation, &videoType, &currentVelocityFrame, &previousVelocityFrame);
}

LIB_MAUIVelocityDllWithAutoInit_CPP_API 
void MW_CALL_CONV check4FirstMovingFrame(int nargout, mwArray& ind, const mwArray& 
                                         currentVelocityFrame, const mwArray& 
                                         previousVelocityFrame, const mwArray& indx)
{
  mclcppMlfFeval(_mcr_inst, "check4FirstMovingFrame", nargout, 1, 3, &ind, &currentVelocityFrame, &previousVelocityFrame, &indx);
}

LIB_MAUIVelocityDllWithAutoInit_CPP_API 
void MW_CALL_CONV processVelocityIntervals(int nargout, mwArray& maxPositive, mwArray& 
                                           avgPositive, mwArray& maxNegative, mwArray& 
                                           avgNegative, mwArray& 
                                           xTrackingLocationIndividual, const mwArray& 
                                           currVelcotiyFrame, const mwArray& frameNum, 
                                           const mwArray& xAxisLocation, const mwArray& 
                                           videoType, const mwArray& firstMovingFrame, 
                                           const mwArray& xTrackingLocationIndividual_in1)
{
  mclcppMlfFeval(_mcr_inst, "processVelocityIntervals", nargout, 5, 6, &maxPositive, &avgPositive, &maxNegative, &avgNegative, &xTrackingLocationIndividual, &currVelcotiyFrame, &frameNum, &xAxisLocation, &videoType, &firstMovingFrame, &xTrackingLocationIndividual_in1);
}

