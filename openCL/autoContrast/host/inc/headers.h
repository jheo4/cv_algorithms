//
//
//	Author : Jin Heo
//	Date : Oct, 3, 2017
//

#include <SDL2/SDL.h>
#include <CL/opencl.h>
#include <algorithm>
#include <iostream>
#include "defines.h"
#include "AOCLUtils/aocl_utils.h"

using namespace aocl_utils;

extern int glutWindowHandle;
extern cl_uint graphOn;

extern cl_uint pixels;

extern int fps_raw;

extern cl_uint *input;
extern cl_uchar *outputGray;
extern cl_uint *output;

extern cl_uint num_platforms;
extern cl_platform_id platform;
extern cl_uint num_devices;
extern cl_device_id device;
extern cl_context context;
extern cl_command_queue queue;
extern cl_program program;

// Kernel definition
extern cl_kernel rgbToYuvKernel;
extern cl_kernel yuvToRgbKernel;
extern cl_kernel equalizeHistogramKernel;

extern cl_mem fpga_buffer[6];

extern std::string inputImg;
extern std::string outputFileName;
extern std::string outputGrayFileName;

extern std::string aocxFilename;
extern std::string deviceInfo;

extern SDL_Window *sdlWindow;
extern SDL_Surface *sdlWindowSurface;
extern SDL_Surface *sdlInputSurface;
extern SDL_Surface *sdlOutputSurface;

extern bool profile;
extern bool useDisplay;
extern bool testMode;
extern unsigned testThresholds[];
extern unsigned testFrameIndex;
