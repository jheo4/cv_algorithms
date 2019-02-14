//
//  Author : Jin Heo
//  Date : Nov, 9, 2017
//  Description : global variables 
//

#include <SDL2/SDL.h>
#include <CL/opencl.h>
#include <algorithm>
#include <iostream>
#include "defines.h"
#include "AOCLUtils/aocl_utils.h"

using namespace aocl_utils;

// FPGA kernel variables
extern cl_kernel gaussianBlurKernel;
extern cl_kernel dogKernel;
extern cl_kernel keyKernel;
extern cl_kernel descKernel;

extern cl_uchar *input;

extern std::string inputImg;

extern int glutWindowHandle;
extern cl_uint graphOn;
extern cl_uint pixels;

extern int fps_raw;

// OpenCL for FPGA configuration varaibles
extern cl_uint num_platforms;
extern cl_platform_id platform;
extern cl_uint num_devices;
extern cl_device_id device;
extern cl_context context;
extern cl_command_queue queue;
extern cl_program program;

extern std::string aocxFilename;
extern std::string deviceInfo;

// SDL variables for displaying the window
extern SDL_Window *sdlWindow;
extern SDL_Surface *sdlWindowSurface;
extern SDL_Surface *sdlInputSurface;
extern SDL_Surface *sdlOutputSurface;

extern bool profile;
extern bool useDisplay;
extern bool testMode;
extern unsigned testThresholds[];
extern unsigned testFrameIndex;

