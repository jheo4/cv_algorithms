//
//  Author : Jin Heo
//  Date : Oct, 3, 2017
//

#include "../host/inc/defines.h"  

#define KERSIZE 5 
#define KERRAD ((KERSIZE-1)/2)

__kernel void cl_gaussianBlur( 
  global uchar * restrict _inputImg, 
  global uchar * restrict _outputImg,
  const float _sigma,
  const int _rows,
  const int _cols
  )
{
  // shifting buffer for 5*5 gaussian blur
  uchar streamBuffer[4*COLS + 5] = {0,};
  
  float gaussianMatrix[25];
  float gaussianSum = 0;
  float result = 0;

  int disX, disY;

  // gaussian Matrix setting...
  for(int i = 0 ; i < KERSIZE ; i++){
    for(int j = 0 ; j < KERSIZE ; j++){
      disX = abs(j-KERRAD);
      disY = abs(i-KERRAD);
      gaussianMatrix[i*KERSIZE+j] = (1 / (2*3.14159*_sigma*_sigma)) / exp( (disX*disX+disY*disY)/(2*_sigma*_sigma) );
      gaussianSum += gaussianMatrix[i*KERSIZE+j];
    }
  }
  for(int i = 0 ; i < 25 ; i++)
    gaussianMatrix[i] = gaussianMatrix[i] / gaussianSum;
  
    
  // pre-load of the stream buffer...
  for (int count = 0; count < 4*_cols + 4 ; count++){
  for(int i = 4*_cols + 4 ; i > 0 ; i--)
    streamBuffer[i] = streamBuffer[i-1];
    streamBuffer[0] = _inputImg[count];
  }
  
  for(int y = KERRAD; y < _rows-KERRAD ; y++){
    for(int x = KERRAD; x < _cols-KERRAD ; x++){
      // shifting buffer
      for(int i = 4*_cols + 4 ; i > 0 ; i--)
        streamBuffer[i] = streamBuffer[i-1];
      // read one pixel
      streamBuffer[0] = _inputImg[(y+KERRAD)*_cols + (x+KERRAD)];
      
      // mutiple image pixels with gaussian blur matrix
    for(int i = 0; i < KERSIZE ; i++)
      for(int j = 0 ; j < KERSIZE ; j++){
        result += (gaussianMatrix[i*KERSIZE + j] * streamBuffer[i*_cols + j]);
      }
    
    // result clamp...
    if(result < 0) _outputImg[y*_cols+x] = 0;
    else if (result > 255) _outputImg[y*_cols+x] = 255;
    else _outputImg[y*_cols+x] = (uchar)result; 
    
    result = 0;
  }
  
  // for y increment, at the very first, read 2 more pixels
  if(y < _rows-KERRAD-1){
    for(int j = 0 ; j < KERSIZE-1; j++){
      for(int i = 4*_cols + 4 ; i > 0 ; i--)
        streamBuffer[i] = streamBuffer[i-1];
      streamBuffer[0] = _inputImg[(y+3)*_cols+j];
      }
    }
  } 
}
