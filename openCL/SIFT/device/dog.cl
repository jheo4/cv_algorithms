//
//  Author : Jin Heo
//  Date : Oct, 16, 2017
//

#include "../host/inc/defines.h"  


// By using Two blurred images, get the difference of Gaussian(DoG)
__kernel void cl_dog( 
    global uchar * restrict _lessBlurred, 
    global uchar * restrict _moreBlurred,
    global uchar * restrict _ouput,
    const int _rows,
    const int _cols)
{
  int resPixel=0;
  for(int i = 0 ; i < _rows * _cols ; i++){
    resPixel = _lessBlurred[i] - _moreBlurred[i];
    if(resPixel < 0) _ouput[i] = 0;
    else if(resPixel > 255)  _ouput[i] = 255;
    else _ouput[i] = (uchar)(resPixel*3);
  }
}

