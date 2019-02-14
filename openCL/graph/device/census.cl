#include "../host/inc/defines.h"

#define CENSUS_HEIGHT 3
#define CENSUS_WIDTH 3

__kernel
void cl_census(
    global short * restrict frame_in,
    global uchar * restrict frame_out,
    const int iterations,
    const unsigned int threshold)
{
  int rows[2 * COLS + 3];
  int count = -(2 * COLS + 3);
  while (count != iterations) {
    #pragma unroll
    for (int i = COLS * 2 + 2; i > 0; --i) {
      rows[i] = rows[i - 1];
    }
    rows[0] = count >= 0 ? (int)frame_in[count] : 255;


    int central_pixel = rows[1 * COLS + 1];
    uchar result = 0;
    #pragma unroll
    for (int i = 0; i < 3; ++i) {
      #pragma unroll
      for (int j = 0; j < 3; ++j) {
        int neighbor_pixel = rows[i * COLS + j];
        if (i != 1 && j != 1) {
          if(neighbor_pixel >= central_pixel)result |= (1u << (i + j));
        }
      }
    }
    uchar clamped;
    clamped = result; // > threshold ? result : 0;
    if (count >= 0) {
      frame_out[count] = clamped;
    }
    count++;
  }
}

