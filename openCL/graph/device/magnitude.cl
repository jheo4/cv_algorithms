//
//    Author : Jin Heo
//    Date : 2017 09 09
//

#include "../host/inc/defines.h"

__kernel void cl_magnitude(
    global short * restrict gradX,
    global short * restrict gradY,
    global short * restrict frame_out,
    const unsigned int iterations
    )
{
  unsigned short x, y;
  unsigned short magnitude;
  #pragma unroll 2
  for(int i = 0 ; i < iterations ; i++){
    x = gradX[i];
    y = gradY[i];
    magnitude = (short)sqrt( (short)((x*x) + (y*y)) );
    frame_out[i] = magnitude;
  }
}

