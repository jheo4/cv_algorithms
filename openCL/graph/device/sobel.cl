#include "../host/inc/defines.h"

__kernel
void cl_sobel(
    global uchar * restrict frame_in,
    global short * restrict frame_out_x,
    global short * restrict frame_out_y,
    const int iterations, const unsigned int threshold)
{
  int Gx[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};
  int Gy[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
  int rows[2 * COLS + 3];
  int count = -(2 * COLS + 3);

  int test = 0;
  while (count != iterations) {
    #pragma unroll
    for (int i = COLS * 2 + 2; i > 0; --i) {
      rows[i] = rows[i - 1];
    }
    rows[0] = count >= 0 ? (int)frame_in[count] : 255;
    int x_dir = 255;
    int y_dir = 255;

    #pragma unroll
    for (int i = 0; i < 3; ++i) {
      #pragma unroll
      for (int j = 0; j < 3; ++j) {
        int pixel = rows[i * COLS + j];
        x_dir -= pixel * Gx[i][j];
        y_dir -= pixel * Gy[i][j];
      }
    }

    int clamped_x, clamped_y;
    clamped_x = abs(x_dir); //> threshold/2 ? abs(x_dir) : 0;
    clamped_y = abs(y_dir); //> threshold/2 ? abs(y_dir) : 0;

    if (count >= 0) {
      frame_out_x[count] = (short)clamped_x;
      frame_out_y[count] = (short)clamped_y;

    }
    count++;
  }
}

#include "census.cl"
#include "magnitude.cl"
#include "histogram.cl"
