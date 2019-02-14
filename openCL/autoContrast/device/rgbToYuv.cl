//
//  Producer : Jin Heo
//  Date : Oct, 3
//  Univ : University of California, Irvine
//

#include "../host/inc/defines.h" 


__kernel void cl_rgbToYuv( 
    global uint * restrict _inputImg, 
    global uchar * restrict _y,
    global uchar * restrict _u,
    global uchar * restrict _v
    )
{
  unsigned int pixel;
  unsigned int r, g, b;
  int tempY, tempU, tempV;

  for(unsigned int i = 0 ; i < ROWS * COLS ; i++){
    pixel = _inputImg[i];
    r = pixel & 0xff;
    g = (pixel >> 8) & 0xff;
    b = (pixel >> 16) & 0xff;
    
    tempY = ((66*r+128*g+25*b+128)>>8)+16;
    tempU = ((-38*r-74*g+112*b+128)>>8)+128;
    tempV = ((112*r-94*g-18*b+128)>>8)+128;

    /*if(tempY > 255) _y[i] = 255; else _y[i] = tempY;
    if(tempU > 255) _u[i] = 255; else _u[i] = tempU;
    if(tempV > 255) _v[i] = 255; else _v[i] = tempV;*/
    _y[i] = tempY;
    _u[i] = tempU;
    _v[i] = tempV;
  }
}
