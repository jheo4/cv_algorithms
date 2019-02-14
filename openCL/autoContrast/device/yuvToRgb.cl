//
//  Producer : Jin Heo
//  Date : Oct, 3
//  Univ : University of California, Irvine
//

#include "../host/inc/defines.h"  
//#define HISTOGRAM 256
//#define WIDTH 
//#define HEIGHT

__kernel void cl_yuvToRgb( 
    global uchar * restrict _y,
    global uchar * restrict _u,
    global uchar * restrict _v,
    global uint * restrict _outputImg)
{
  int c, d, e;
  int tempR, tempG, tempB;
  uchar r, g, b;
  uint pixel=0;
  for(unsigned int i = 0 ; i < ROWS*COLS ; i++){
    c = _y[i]-16;
    d = _u[i]-128;
    e = _v[i]-128;
    tempR = ((298*c+409*e+128)>>8);
    tempG = ((298*c-100*d-208*e+128)>>8);
    tempB = ((298*c+516*d+128)>>8);
    
    r = (uchar)clamp((int)tempR, 0, 255);
    g = (uchar)clamp((int)tempG, 0, 255);
    b = (uchar)clamp((int)tempB, 0, 255); 
    
    pixel = b;
    pixel = (pixel << 8) + g;
    pixel = (pixel << 8) + r;
    _outputImg[i] = pixel;
  }
}
