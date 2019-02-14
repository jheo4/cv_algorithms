//
//  Producer : Jin Heo
//  School : University of California
//  Data : Sep 15
//
#include "headers.h"
#include "pgmreader.h"
#include "demo.h"

// Graph Execution...
void graphExe()
{
  clSetKernelArg(stereoKernel, 0, sizeof(cl_mem), &fpga_buffer[0]);
  clSetKernelArg(stereoKernel, 1, sizeof(cl_mem), &fpga_buffer[1]);
  clSetKernelArg(stereoKernel, 2, sizeof(cl_mem), &fpga_buffer[2]);

  clEnqueueWriteBuffer(queue, fpga_buffer[0], CL_FALSE, 0,
      sizeof(cl_uchar) * ROWS * COLS, inputL, 0, NULL, NULL);
  clFinish(queue);

  clEnqueueWriteBuffer(queue, fpga_buffer[1], CL_FALSE, 0,
      sizeof(cl_uchar) * ROWS * COLS, inputR, 0, NULL, NULL);
  clFinish(queue);

  cl_event event;

  clEnqueueTask(queue, stereoKernel, 0, NULL, &event);
  clFinish(queue);
  printf("zot1\r\n");
  // FPS calc
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

  // Get result from FPGA
  clEnqueueReadBuffer(queue, fpga_buffer[2], CL_FALSE, 0,
      sizeof(cl_uchar)*(RES_ROWS)*(RES_COLS), output, 0, NULL, NULL);
  clFinish(queue);

  printf("zot2\r\n");
  // Write the result file
  pgmWriter(outputFileName.c_str(), (unsigned char*)output);
  printf("zot3\r\n");

  // Disp setting
  /*for(int i = 0 ; i < PAD_ROWS*PAD_COLS ; i++)
    calcToDisp(disp_output, output, i);
  */
  printf("zot4\r\n");

  // Window Title bar
  char title[256];
  sprintf(title, "Filtering (%d FPS)", fps_raw);
  SDL_SetWindowTitle(sdlWindow, title);
}

