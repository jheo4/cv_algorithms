//
//  Author : Jin Heo
//  Date : Oct, 26, 2017
//
#include "../host/inc/defines.h"  

__kernel void cl_keyDescriptor( 
    global uchar * restrict _inputImg, 
    const float _sigma,
    global keypoint * restrict _inputKey,
    global keypoint * restrict _outputKey,
    global keypointDesc * restrict _outputDesc,
    const int _rows,
    const int _cols)
{
  // image buffer 18*18 for Dx, Dy of 16*16
  uchar buf[18][18];
  float mag[16][16];
  float theta[16][16];
  
  int cX, cY;
  
  float degreeConv = 180/3.1416;
  int degree;
  float bins36[36] = {0,};
  float bins8[8] = {0,};
  uchar binIndex = 0;
  
  float gaussianMatrix[16][16];
  float gaussianSum;
  
  // Gaussian Matrix for magnitude...
  for(int y = 0 ; y < 16; y++){
    for(int x = 0 ; x < 16; x++){
      gaussianMatrix[y][x] = 0;
      uint distX = abs(x-8);
      uint distY = abs(y-8);
      gaussianMatrix[y][x] = (1 / (2 * 3.14159*_sigma*_sigma)) / exp( ( (distX*distX + distY*distY) / (2 * _sigma*_sigma) ) );
          gaussianSum += gaussianMatrix[y][x];
    }
  }  
  for(int y = 0 ; y < 16; y++)
    for(int x = 0 ; x < 16; x++)
      gaussianMatrix[y][x] /= gaussianSum;
      
  uint numOfInputKeys = _inputKey->numOfKeys;
  uint numOfOutputKeys = 0;
  
  // for each key, make a descriptor...
  for(int i = 0 ; i < numOfInputKeys; i++){
    // init...
    for(int y = 0 ; y < 16; y++){
      for(int x = 0 ; x < 16; x++){
        mag[y][x] = 0;
        theta[y][x] = 0;
      }
    }
    for(int j = 0 ; j < 36 ; j++) bins36[j] = 0;
    for(int j = 0 ; j < 8 ; j++) bins8[j] = 0;
    
    // key position...
    int kX = _inputKey->x[i];
    int kY = _inputKey->y[i];
  
    // buf is 18*18, so bolder policy is needed
    if(kX > 7 && kX < _cols-9 && kY > 7 && kY < _rows-9){
      // load an image patch 
      for(int y = -8 ; y < 10 ; y++){
        for(int x = -8 ; x < 10 ; x++){
          buf[y+8][y+8] = _inputImg[(y+kY)*_cols + (x+kX)];
        }
      }
      
      // get theta & magni
      for(int y = 1; y < 17; y++){
        for(int x = 1; x < 17; x++){
          // calc mag, theta
          cX = (buf[y][x+1] - buf[y][x-1]);
          cY = (buf[y+1][x] - buf[y-1][x]);
          mag[y-1][x-1] = sqrt(cX*cX + cY*cY);
          theta[y-1][x-1] = atan2(cY, cX);  
        }
      }
      
      for(int y = 0 ; y < 16; y++)
        for(int x = 0 ; x < 16 ; x++)
          mag[y][x] *= gaussianMatrix[y][x];
    
      // 36 bins with gaussianed magnitude by using theta 
      for(int y = 0 ; y < 16; y++){
        for(int x = 0 ; x < 16 ; x++){
          // theta -> degree
          degree = (int)(degreeConv * theta[y][x]);  
          if(degree < 0) degree+=359;
          
          if(degree < 10) binIndex = 0; else if(degree < 20) binIndex = 1; else if(degree < 30) binIndex = 2; else if(degree < 40) binIndex = 3;
          else if(degree < 50) binIndex = 4; else if(degree < 60) binIndex = 5; else if(degree < 70) binIndex = 6; else if(degree < 80) binIndex = 7;
          else if(degree < 90) binIndex = 8; else if(degree < 100) binIndex = 9; else if(degree < 110) binIndex = 10;
          else if(degree < 120) binIndex = 11; else if(degree < 130) binIndex = 12; else if(degree < 140) binIndex = 13; else if(degree < 150) binIndex = 14;
          else if(degree < 160) binIndex = 15; else if(degree < 170) binIndex = 16; else if(degree < 180) binIndex = 17;  else if(degree < 190) binIndex = 18;
          else if(degree < 200) binIndex = 19;  else if(degree < 210) binIndex = 20;  else if(degree < 220) binIndex = 21;  else if(degree < 230) binIndex = 22;
          else if(degree < 240) binIndex = 23;  else if(degree < 250) binIndex = 24;  else if(degree < 260) binIndex = 25;  else if(degree < 270) binIndex = 26;
          else if(degree < 280) binIndex = 27;  else if(degree < 290) binIndex = 28;  else if(degree < 300) binIndex = 29;  else if(degree < 310) binIndex = 30;
          else if(degree < 320) binIndex = 31;  else if(degree < 330) binIndex = 32;  else if(degree < 340) binIndex = 33;  else if(degree < 350) binIndex = 34;
          else if(degree < 360) binIndex = 35;
        
          bins36[binIndex] += mag[y][x];  
        }
      }
        
      // keypoint orientation (36 directions)
      float maxMag36 = bins36[0];
      for(int j = 1 ; j < 36; j++)
        if(bins36[j] > maxMag36) maxMag36 = bins36[j];
      
      for(int j = 0 ; j < 36; j++){
        if(bins36[j] == maxMag36){
          _outputKey->x[numOfOutputKeys] = kX;
          _outputKey->y[numOfOutputKeys] = kY;
          _outputKey->orientation[numOfOutputKeys] = j+1;
        }
      }
    
      // Key points descriptor...
      // 16*16 -> 16 * (4*4 rectangle)
      int idxY, idxX;  // for the box index
      int gY, gX;  // global y, x 
      uchar tempDir[16];
      uchar tempMag[16];
      
      for(int j = 0 ; j < 16 ; j++){
        idxY = j / 4;
        idxX = j % 4;
        // bins8 for each descriptor
        for(int y = 0 ; y < 4; y++){
          for(int x = 0 ; x < 4; x++){
            gY = y + (idxY*4);
            gX = x + (idxX*4);
            
            // theta -> degree
            degree = (int)(degreeConv * theta[gY][gX]);  
            if(degree < 0) degree+=359;
            
            // 8 directions
            if(degree < 45) binIndex = 0;
            else if(degree < 90) binIndex = 1;
            else if(degree < 135) binIndex = 2;
            else if(degree < 180) binIndex = 3;
            else if(degree < 225) binIndex = 4;
            else if(degree < 270) binIndex = 5;
            else if(degree < 315) binIndex = 6;
            else if(degree < 360) binIndex = 7;
            bins8[binIndex] += mag[gY][gX];
          }
        }
        float maxMag8 = bins8[0];
        for(int k = 1 ; k < 8; k++)
          if(maxMag8 < bins8[k]) maxMag8 = bins8[k];
        
        // descriptor info -> direction and magnitude
        for(int k = 0 ; k < 8; k++){
          if(maxMag8 == bins8[k]){
            tempDir[j] = k+1;
            if(maxMag8/maxMag36 > 1) tempMag[j] = 255;
            else tempMag[j] = (maxMag8/maxMag36)*255;
            _outputDesc->dir[numOfOutputKeys][j] = tempDir[j];
            _outputDesc->mag[numOfOutputKeys][j] = tempMag[j];
          }
        }
      }
      numOfOutputKeys++;
      
      // additional keypoints...
      // 0.8 ~ 1 magnitude -> new key with other direction...
      for(int k = 0 ; k < 36 ; k++){
        bins36[k] = bins36[k] / maxMag36;
        if(bins36[k] >= 0.8 && bins36[k] < 1){
          _outputKey->x[numOfOutputKeys] = kX;
          _outputKey->y[numOfOutputKeys] = kY;
          _outputKey->orientation[numOfOutputKeys] = k+1;
          for(int l = 0 ; l < 16 ; l++){
            _outputDesc->dir[numOfOutputKeys][l] = tempDir[l];
            _outputDesc->mag[numOfOutputKeys][l] = tempMag[l];
          }
          numOfOutputKeys++;
        }
      }
    }    
  }
  _outputKey->numOfKeys = numOfOutputKeys;
}
