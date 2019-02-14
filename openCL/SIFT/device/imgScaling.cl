//
//  Author : Jin Heo
//  Date : Oct, 3, 2017
//

#include "../host/inc/defines.h"

// n*n -> n/2 * n/2 image
__kernel void cl_imgDownSampling( 
  global uchar * restrict _inputImg, 
  global uchar * restrict _outputImg,
  const int _rows,
  const int _cols)
{
  for(int i = 0 ; i < _rows; i++){
  for(int j = 0 ; j < _cols ; j++){
    _outputImg[i*_cols+j] = _inputImg[(i*2)*(_cols*2)+(j*2)];
  }
  }
}

// n*n -> n*2 * n*2 image
__kernel void cl_imgUpSampling(
  global uchar * restrict _inputImg,
  global uchar * restrict _outputImg,
  const int _rows,
  const int _cols){
  uchar inputPixel;
  for(uint i = 0 ; i < _rows ; i++){
  for(uint j = 0 ; j < _cols ; j++){
    inputPixel = _inputImg[i*COLS+j];
    _outputImg[(i*2)*(_cols*2) + (j*2)] = inputPixel;
    _outputImg[((i*2)+1)*(_cols*2) + (j*2)] = inputPixel;
    _outputImg[(i*2)*(_cols*2) + (j*2)+1] = inputPixel;
    _outputImg[ ((i*2)+1)*(_cols*2)+ (j*2)+1 ] = inputPixel;
  }
  }
}

#include "gaussianBlur.cl"
#include "dog.cl"
#include "keyPoint.cl"
#include "keyDescriptor.cl"
