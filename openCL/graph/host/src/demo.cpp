//
//    Author : Jin Heo
//    Date : 2017 09 09
//

#include "headers.h"
#include "pgmreader.h"
#include "demo.h"

void graphExe()
{
  clSetKernelArg(sobelKernel, 0, sizeof(cl_mem), &fpga_buffer[0]);
  clSetKernelArg(sobelKernel, 1, sizeof(cl_mem), &fpga_buffer[1]);
  clSetKernelArg(sobelKernel, 2, sizeof(cl_mem), &fpga_buffer[2]);
  clSetKernelArg(sobelKernel, 3, sizeof(cl_uint), &pixels);
  clSetKernelArg(sobelKernel, 4, sizeof(unsigned int), &thresh);

  clSetKernelArg(magnitudeKernel, 0, sizeof(cl_mem), &fpga_buffer[1]);
  clSetKernelArg(magnitudeKernel, 1, sizeof(cl_mem), &fpga_buffer[2]);
  clSetKernelArg(magnitudeKernel, 2, sizeof(cl_mem), &fpga_buffer[3]);
  clSetKernelArg(magnitudeKernel, 3, sizeof(cl_uint), &pixels);

  clSetKernelArg(censusKernel, 0, sizeof(cl_mem), &fpga_buffer[3]);
  clSetKernelArg(censusKernel, 1, sizeof(cl_mem), &fpga_buffer[4]);
  clSetKernelArg(censusKernel, 2, sizeof(cl_uint), &pixels);
  clSetKernelArg(censusKernel, 3, sizeof(cl_uint), &pixels);

  clSetKernelArg(histogramKernel , 0, sizeof(cl_mem), &fpga_buffer[4]);
  clSetKernelArg(histogramKernel , 1, sizeof(cl_mem), &fpga_buffer[5]);
  clSetKernelArg(histogramKernel , 2, sizeof(cl_mem), &bins_buffer);
  clSetKernelArg(histogramKernel , 3, sizeof(cl_uint), &pixels);


  clEnqueueWriteBuffer(queue, fpga_buffer[0], CL_FALSE, 0,
      sizeof(cl_uchar) * ROWS * COLS, input, 0, NULL, NULL);
  clFinish(queue);

  cl_event event;
  clEnqueueTask(queue, sobelKernel, 0, NULL, &event);
  clFinish(queue);
  clEnqueueTask(queue, magnitudeKernel, 0, NULL, &event);
  clFinish(queue);
  clEnqueueTask(queue, censusKernel, 0, NULL, &event);
  clFinish(queue);
  clEnqueueTask(queue, histogramKernel, 0, NULL, &event);
  clFinish(queue);


  // Profile FPS
  cl_ulong start, end;
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong),
      &start, NULL);
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong),
      &end, NULL);
  clReleaseEvent(event);
  fps_raw = (int)(1.0f / ((end - start) * 1e-9f));
  if (profile) {
    printf("Throughput: %d FPS\n", fps_raw);
  }

  clEnqueueReadBuffer(queue, fpga_buffer[5], CL_FALSE, 0,
      sizeof(cl_uchar) * ROWS * COLS, output, 0, NULL, NULL);
  clFinish(queue);
  // For printing histogram...
  clEnqueueReadBuffer(queue, bins_buffer, CL_FALSE, 0,
      sizeof(unsigned int) * 256, bins, 0, NULL, NULL);
  clFinish(queue);

  pgmWriter(outputFileName.c_str(), COLS, ROWS, (unsigned char*)output);
  // Window Title bar
  char title[256];
  sprintf(title, "Filtering (%d FPS)", fps_raw);
  SDL_SetWindowTitle(sdlWindow, title);


  for(int i = 0 ; i < HISTOGRAM; i++){
    printf("%d ", bins[i]);
    bins[i] = 0;
  }
}
