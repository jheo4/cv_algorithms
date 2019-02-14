//
//
//  Author : Jin Heo
//  Date : Oct, 3, 2017
//

#include "headers.h"
#include "fileReader.h"
#include "demo.h"

void graphExe()
{
  // inputIMG, Y, U, V
  clSetKernelArg(rgbToYuvKernel, 0, sizeof(cl_mem), &fpga_buffer[0]);
  clSetKernelArg(rgbToYuvKernel, 1, sizeof(cl_mem), &fpga_buffer[1]);
  clSetKernelArg(rgbToYuvKernel, 2, sizeof(cl_mem), &fpga_buffer[2]);
  clSetKernelArg(rgbToYuvKernel, 3, sizeof(cl_mem), &fpga_buffer[3]);

  // Y, output
  clSetKernelArg(equalizeHistogramKernel, 0, sizeof(cl_mem), &fpga_buffer[1]);
  clSetKernelArg(equalizeHistogramKernel, 1, sizeof(cl_mem), &fpga_buffer[4]);

  // Y, U, V, output IMG
  clSetKernelArg(yuvToRgbKernel, 0, sizeof(cl_mem), &fpga_buffer[4]);
  clSetKernelArg(yuvToRgbKernel, 1, sizeof(cl_mem), &fpga_buffer[2]);
  clSetKernelArg(yuvToRgbKernel, 2, sizeof(cl_mem), &fpga_buffer[3]);
  clSetKernelArg(yuvToRgbKernel, 3, sizeof(cl_mem), &fpga_buffer[5]);

  clEnqueueWriteBuffer(queue, fpga_buffer[0], CL_FALSE, 0,
      sizeof(cl_uint) * ROWS * COLS, input, 0, NULL, NULL);
  clFinish(queue);

  cl_event event;

  clEnqueueTask(queue, rgbToYuvKernel, 0, NULL, &event);
  clFinish(queue);

  clEnqueueTask(queue, equalizeHistogramKernel, 0, NULL, &event);
  clFinish(queue);

  clEnqueueTask(queue, yuvToRgbKernel, 0, NULL, &event);
  clFinish(queue);

  // Get result from FPGA
  clEnqueueReadBuffer(queue, fpga_buffer[4], CL_FALSE, 0,
      sizeof(cl_uchar)*(ROWS)*(COLS), outputGray, 0, NULL, NULL);
  clFinish(queue);

  clEnqueueReadBuffer(queue, fpga_buffer[5], CL_FALSE, 0,
      sizeof(cl_uint)*(ROWS)*(COLS), output, 0, NULL, NULL);
  clFinish(queue);

  // Write the result file
  pgmWriter(outputGrayFileName.c_str(), (unsigned char*)outputGray);
  ppmWriter(outputFileName.c_str(), (unsigned int*)output);

  // FPS calc
  cl_ulong start, end;
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START,
      sizeof(cl_ulong), &start, NULL);
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END,
      sizeof(cl_ulong), &end, NULL);
  clReleaseEvent(event);
  fps_raw = (int)(1.0f / ((end - start) * 1e-9f));
  if (profile) {
    printf("Throughput: %d FPS\n", fps_raw);
  }

  // Window Title bar
  char title[256];
  sprintf(title, "Filtering (%d FPS)", fps_raw);
  SDL_SetWindowTitle(sdlWindow, title);
}

