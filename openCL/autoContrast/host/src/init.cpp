//
//
//  Author : Jin Heo
//  Date : Oct, 3, 2017
//

#include "defines.h"
#include "init.h"
#include "headers.h"

int glutWindowHandle;
int fps_raw = 0;
cl_uint *input = NULL;
cl_uchar *outputGray = NULL;
cl_uint *output = NULL;


cl_uint num_platforms;
cl_platform_id platform;
cl_uint num_devices;
cl_device_id device;
cl_context context;
cl_command_queue queue;
cl_program program;

cl_kernel rgbToYuvKernel;
cl_kernel yuvToRgbKernel;
cl_kernel equalizeHistogramKernel;

cl_mem fpga_buffer[6];

std::string inputImg;
std::string outputFileName;
std::string outputGrayFileName;

std::string aocxFilename;
std::string deviceInfo;

SDL_Window *sdlWindow = NULL;
SDL_Surface *sdlWindowSurface = NULL;
SDL_Surface *sdlInputSurface = NULL;
SDL_Surface *sdlOutputSurface = NULL;

bool profile = false;
bool useDisplay = true;
bool testMode = false;
unsigned testThresholds[] = {32, 96, 128, 192, 225};
unsigned testFrameIndex = 0;
cl_uint pixels = COLS * ROWS;
cl_uint graphOn = 0;


int initSDL()
{
  if(SDL_Init(useDisplay ? SDL_INIT_VIDEO : 0) != 0) {
    std::cerr << "Error: Could not initialize SDL: " << SDL_GetError() << "\n";
    return false;
  }

  if(useDisplay) {
    sdlWindow = SDL_CreateWindow("Graph Application",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        641, 641 * ROWS / COLS, 0);

    sdlWindowSurface = SDL_GetWindowSurface(sdlWindow);
    sdlInputSurface = SDL_CreateRGBSurfaceFrom(input, COLS, ROWS, 32,
        COLS * sizeof(unsigned int), 0xff, 0xff00, 0xff0000, 0);
    sdlOutputSurface = SDL_CreateRGBSurfaceFrom(output, COLS, ROWS, 32,
        COLS * sizeof(unsigned int), 0xff, 0xff00, 0xff0000, 0);
  }
  return true;
}

void initCL()
{
  cl_int status;

  if (!setCwdToExeDir()) {
    teardown(0);
  }

  platform = findPlatform("Intel(R) FPGA");
  status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);
  num_devices = 1; // always only using one device

  char info[256];
  clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(info), info, NULL);

  context = clCreateContext(0, num_devices, &device, &oclContextCallback,
      NULL, &status);

  queue = clCreateCommandQueue(context, device, 0, &status);

  std::string binary_file = getBoardBinaryFile("sobel", device);
  std::cout << "Using AOCX: " << binary_file << "\n";
  program = createProgramFromBinary(context, binary_file.c_str(), &device, 1);

  status = clBuildProgram(program, num_devices, &device, "", NULL, NULL);

  rgbToYuvKernel = clCreateKernel(program, "cl_rgbToYuv", &status);
  yuvToRgbKernel = clCreateKernel(program, "cl_yuvToRgb", &status);
  equalizeHistogramKernel = clCreateKernel(program, "cl_equalizeHistogram",
      &status);

  fpga_buffer[0] = clCreateBuffer(context, CL_MEM_READ_ONLY,
      sizeof(cl_uint) * ROWS * COLS, NULL, &status);
  fpga_buffer[1] = clCreateBuffer(context, CL_MEM_READ_WRITE,
      sizeof(cl_uchar) * ROWS * COLS, NULL, &status);
  fpga_buffer[2] = clCreateBuffer(context, CL_MEM_READ_WRITE,
      sizeof(cl_uchar) * ROWS * COLS, NULL, &status);
  fpga_buffer[3] = clCreateBuffer(context, CL_MEM_READ_WRITE,
      sizeof(cl_uchar) * ROWS * COLS, NULL, &status);
  fpga_buffer[4] = clCreateBuffer(context, CL_MEM_READ_WRITE,
      sizeof(cl_uchar) * ROWS * COLS, NULL, &status);
  fpga_buffer[5] = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
      sizeof(cl_uint) * ROWS * COLS, NULL, &status);
}


void teardown(int exit_status)
{
  if (input) alignedFree(input);
  if (output) alignedFree(output);
  if (outputGray) alignedFree(outputGray);
  for(int i = 0 ; i < 6; i++)
    if(fpga_buffer[i]) clReleaseMemObject(fpga_buffer[i]);

  if (rgbToYuvKernel) clReleaseKernel(rgbToYuvKernel);
  if (yuvToRgbKernel) clReleaseKernel(yuvToRgbKernel);
  if (equalizeHistogramKernel) clReleaseKernel(equalizeHistogramKernel);

  if (program) clReleaseProgram(program);
  if (queue) clReleaseCommandQueue(queue);
  if (context) clReleaseContext(context);

  SDL_Quit();
  exit(exit_status);
}

void cleanup(){
  teardown(-1);
}

void dumpFrame(unsigned frameIndex, unsigned *frameData) {
  char fname[256];
  sprintf(fname, "frame%d.ppm", frameIndex);
  printf("Dumping %s\n", fname);

  FILE *f = fopen(fname, "wb");
  fprintf(f, "P6 %d %d %d\n", COLS, ROWS, 255);
  for(unsigned y = 0; y < ROWS; ++y) {
    for(unsigned x = 0; x < COLS; ++x) {
      // This assumes byte-order is little-endian.
      unsigned pixel = frameData[y * COLS + x];
      fwrite(&pixel, 1, 3, f);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}
