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

extern cl_uchar *inputL;
extern cl_uchar *inputR;
extern cl_uint *disp_input;
extern cl_uchar *output;
extern cl_uint *disp_output;

extern cl_uint num_platforms;
extern cl_platform_id platform;
extern cl_uint num_devices;
extern cl_device_id device;
extern cl_context context;
extern cl_command_queue queue;
extern cl_program program;

// Kernel definition
extern cl_kernel stereoKernel;

extern cl_mem fpga_buffer[3];

extern std::string inputImgL;
extern std::string inputImgR;
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
