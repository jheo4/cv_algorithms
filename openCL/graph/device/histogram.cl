//
//    Author : Jin Heo
//    Date : 2017 09 09
//

#include "../host/inc/defines.h"

__kernel void cl_histogram(
    global uchar * restrict frame_in,
    global uchar * restrict frame_out,
    global unsigned int * restrict bins,
    const unsigned int iterations
    ){
  uchar pixel;
  unsigned int tempBins[HISTOGRAM]={0,};

  unsigned int test1 = 0;

  for(unsigned int i = 0 ; i < iterations ; i++){
    pixel = frame_in[i];
    tempBins[pixel]++;
    frame_out[i]=pixel;
  }

  for(int i = 0 ; i < HISTOGRAM ; i++){
    bins[i] = tempBins[i];
  }
}

