//
//  Author : Jin Heo
//  Date : Oct, 3, 2017
//

#include "../host/inc/defines.h"

__kernel void cl_equalizeHistogram(
    global uchar * restrict _inputImg,
    global uchar * restrict _outputImg)
{
  unsigned int histogram[HISTOGRAM] = {0,};
  uchar cdf[HISTOGRAM] = {0,};
  unsigned int cdfSum = 0;

  float normFactor = 255.0f/(float)(ROWS*COLS);

  for(unsigned int i = 0 ; i < ROWS*COLS ; i++){
    histogram[_inputImg[i]]++;
  }
  unsigned int sum = 0;
  for(int i = 0 ; i < HISTOGRAM; i++)
    sum += histogram[i];
  for(unsigned int i = 0 ; i < HISTOGRAM ; i++){
    cdfSum+=histogram[i];

    // normalized cdf...
    cdf[i] = (uchar)(cdfSum * normFactor); 
  }

  for(unsigned int i = 0 ; i < ROWS * COLS ; i++){
    _outputImg[i] = cdf[_inputImg[i]]; 
  }
}

#include "yuvToRgb.cl"
#include "rgbToYuv.cl"
