#include <SDL2/SDL.h>
#include <CL/opencl.h>
#include <algorithm>
#include <iostream>
#include "defines.h"
#include "AOCLUtils/aocl_utils.h"

using namespace aocl_utils;

extern int glutWindowHandle;
extern cl_uint graphOn ;
extern cl_uint pixels;
extern cl_uint graphOn;
extern cl_uint histogram[256];

extern cl_uint thresh ;
extern int fps_raw;

extern cl_uchar *input;
extern cl_uint *disp_input;
extern cl_uchar *output;
extern cl_uint *disp_output;
extern cl_uint bins[HISTOGRAM];

extern cl_uint num_platforms;
extern cl_platform_id platform;
extern cl_uint num_devices;
extern cl_device_id device;
extern cl_context context;
extern cl_command_queue queue;
extern cl_program program;

// Kernel definition
extern cl_kernel sobelKernel;
extern cl_kernel magnitudeKernel;
extern cl_kernel censusKernel;
extern cl_kernel histogramKernel;

extern cl_mem fpga_buffer[6];
extern cl_mem bins_buffer;

extern std::string inputFileName;
extern std::string outputFileName;
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
