//
//  Author : Jin Heo
//  Date : Nov, 9, 2017
//

#include "defines.h"
#include "init.h"
#include "headers.h"
// declare the extern kernel variables
cl_kernel gaussianBlurKernel;
cl_kernel dogKernel;
cl_kernel keyKernel;
cl_kernel descKernel;

cl_uchar *input = NULL;

std::string inputImg;

std::string aocxFilename;
std::string deviceInfo;

int glutWindowHandle;
int fps_raw = 0;

cl_uint num_platforms;
cl_platform_id platform;
cl_uint num_devices;
cl_device_id device;
cl_context context;
cl_command_queue queue;
cl_program program;

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
        COLS, ROWS, 0);
    sdlWindowSurface = SDL_GetWindowSurface(sdlWindow);
    sdlInputSurface = SDL_CreateRGBSurfaceFrom(input, COLS, ROWS, 32,
        COLS * sizeof(unsigned char), 0xff, 0xff00, 0xff0000, 0);
    sdlOutputSurface = SDL_CreateRGBSurfaceFrom(input, COLS, ROWS, 32,
        COLS * sizeof(unsigned char), 0xff, 0xff00, 0xff0000, 0);
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

  context = clCreateContext(0, num_devices, &device, &oclContextCallback, NULL,
      &status);

  queue = clCreateCommandQueue(context, device, 0, &status);

  std::string binary_file = getBoardBinaryFile("sobel", device);
  std::cout << "Using AOCX: " << binary_file << "\n";
  program = createProgramFromBinary(context, binary_file.c_str(), &device, 1);

  status = clBuildProgram(program, num_devices, &device, "", NULL, NULL);

  // mapping of FPGA kernel functions and host kernel variables 
  gaussianBlurKernel = clCreateKernel(program, "cl_gaussianBlur", &status);
  dogKernel = clCreateKernel(program, "cl_dog", &status);
  keyKernel = clCreateKernel(program, "cl_keyPoint", &status);
  descKernel = clCreateKernel(program, "cl_keyDescriptor", &status);
}

// Release the mapping
void teardown(int exit_status)
{
  if (gaussianBlurKernel) clReleaseKernel(gaussianBlurKernel);
  if (dogKernel) clReleaseKernel(dogKernel);
  if (keyKernel) clReleaseKernel(keyKernel);
  if (descKernel) clReleaseKernel(descKernel);

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
      unsigned pixel = frameData[y * COLS + x];
      fwrite(&pixel, 1, 3, f);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}
