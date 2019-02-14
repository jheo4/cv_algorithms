//
//  Author : Jin Heo
//  Date : Oct, 26, 2017
//

#include "../host/inc/defines.h"  

__kernel void cl_keyPoint( 
  // Dog...
  global uchar * restrict _upperLayer, 
  global uchar * restrict _midLayer,
  global uchar * restrict _lowerLayer,
  
  // image...
  global uchar * restrict _upperImg,
  global uchar * restrict _midImg,
  global uchar * restrict _lowerImg,    
  
  global keypoint * restrict _keypoints,
  const int _rows,
  const int _cols)
{
  // for local extrema of Dog...
  uchar upperBuf[2*COLS+3]={0,};
  uchar midBuf[2*COLS+3]={0,};
  uchar lowerBuf[2*COLS+3]={0,};
  
  // at the extrema, get the image pixels for derivatives
  uchar upperImg[3][3];
  uchar midImg[3][3];
  uchar lowerImg[3][3];
  
  bool isKeyPoint, isCandidatePoint;
  uchar compPixel=0;
  int offsetY, offsetX, offsetS;
  
  // for keypoint localization...
  //
  float imgScale = 0.00392; // 1/255
  float derivScale = 0.00392; // 1/510
  float secondDerivScale = 0.00196; // 1/255
  float crossDerivScale = 0.00392; // 1/1020
  
  float Dx=0, Dy=0, Ds=0;
  float Dxx=0, Dyy=0, Dss=0, Dxy=0, Dxs=0, Dys=0;
  float cur=0;
  float xhatX=0, xhatY=0, xhatS=0; 
  
  
  float nDx=0, nDy=0, nDs=0;
  float nDxx=0, nDxy=0, nDyy=0;
  float nCur=0;
  float contrast=0;
  float trace=0, det=0;
  
  uint numOfKeys = 0;

  // pre-loading at the very first time
  // in the loop, read only one pixel -> 2*cols + 3
  for(int i = 0; i < _cols*2  ; i++){
    for(int j = _cols*2+2 ; j > 0; j--){
      upperBuf[j] = upperBuf[j-1];
      midBuf[j] = midBuf[j-1];
      lowerBuf[j] = lowerBuf[j-1];
    }
    upperBuf[0] = _upperLayer[i];
    midBuf[0] = _midLayer[i];
    lowerBuf[0] = _lowerLayer[i];
  }
  
  
  for(int y = 1; y < _rows - 1 ; y++){
    // 3 shifts for the row increment
    for(int i = 0 ; i < 3; i++){
      for(int i = _cols*2+2 ; i > 0; i--){
        upperBuf[i] = upperBuf[i-1];
        midBuf[i] = midBuf[i-1];
        lowerBuf[i] = lowerBuf[i-1];
      } 
      upperBuf[0] = _upperLayer[(y+1)*_cols + i];
      midBuf[0] = _midLayer[(y+1)*_cols + i];
      lowerBuf[0] = _lowerLayer[(y+1)*_cols + i];
    }
    
    
    for(int x = 1 ; x < _cols - 1 ; x++){
      compPixel = midBuf[_cols+1]; 
      isCandidatePoint = false;
      
      // find local extrema of DoG
      if(compPixel > upperBuf[0] && compPixel > upperBuf[1] && compPixel > upperBuf[2] && compPixel > upperBuf[_cols] && compPixel > upperBuf[_cols+1] && compPixel > upperBuf[_cols+2] && 
        compPixel > upperBuf[_cols*2] && compPixel > upperBuf[_cols*2+1] && compPixel > upperBuf[_cols*2+2] && compPixel > lowerBuf[0] && compPixel > lowerBuf[1] && 
        compPixel > lowerBuf[2] && compPixel > lowerBuf[_cols] && compPixel > lowerBuf[_cols+1] && compPixel > lowerBuf[_cols+2] && compPixel > lowerBuf[_cols*2] && 
        compPixel > lowerBuf[_cols*2+1] && compPixel > lowerBuf[_cols*2+2] && compPixel > midBuf[0] && compPixel > midBuf[1] && compPixel > midBuf[2] && compPixel > midBuf[_cols] && 
        compPixel > midBuf[_cols+2] && compPixel > midBuf[_cols*2] && compPixel > midBuf[_cols*2+1] && compPixel > midBuf[_cols*2+2] ){
        isCandidatePoint = true;
      }
      else if(compPixel < upperBuf[0] && compPixel < upperBuf[1] && compPixel < upperBuf[2] && compPixel < upperBuf[_cols] && compPixel < upperBuf[_cols+1] && compPixel < upperBuf[_cols+2] && 
      compPixel < upperBuf[_cols*2] && compPixel < upperBuf[_cols*2+1] && compPixel < upperBuf[_cols*2+2] && compPixel < lowerBuf[0] && compPixel < lowerBuf[1] && 
      compPixel < lowerBuf[2] && compPixel < lowerBuf[_cols] && compPixel < lowerBuf[_cols+1] && compPixel < lowerBuf[_cols+2] && compPixel < lowerBuf[_cols*2] && 
      compPixel < lowerBuf[_cols*2+1] && compPixel < lowerBuf[_cols*2+2] && compPixel < midBuf[0] && compPixel < midBuf[1] && compPixel < midBuf[2] && compPixel < midBuf[_cols] && 
      compPixel < midBuf[_cols+2] && compPixel < midBuf[_cols*2] && compPixel < midBuf[_cols*2+1] && compPixel < midBuf[_cols*2+2]){
        isCandidatePoint = true;
      }
      
      // At the local extrema, get the derivatives of the image
      if(isCandidatePoint){ 
        offsetX = 0; 
        offsetY = 0;
        offsetS = 0;
        
        for(int hatCounter = 0 ; hatCounter < 3 ; hatCounter++){
          // Load the image patch...
          for(int tempY = 0; tempY < 3; tempY++){
            for(int tempX = 0; tempX < 3; tempX++){
              upperImg[tempY][tempX] = _upperImg[ (y+tempY-1 + offsetY)*_cols + (x+tempX-1 + offsetX) ];
              midImg[tempY][tempX] = _midImg[ (y+tempY-1 + offsetY)*_cols + (x+tempX-1 + offsetX) ];
              lowerImg[tempY][tempX] = _lowerImg[ (y+tempY-1 + offsetY)*_cols + (x+tempX-1 + offsetX) ];
            }
          }
        
          Dx = (midImg[1][2] - midImg[1][0]) * derivScale;
          Dy = (midImg[2][1] - midImg[0][1]) * derivScale;
          Ds = (upperImg[1][1] - lowerImg[1][1]) * derivScale;
          cur = midImg[1][1] * 2;

          Dxx = (midImg[1][2] + midImg[1][0] - cur) * secondDerivScale;
          Dyy = (midImg[2][1] + midImg[0][1] - cur) * secondDerivScale;
          Dss = (upperImg[1][1] + lowerImg[1][1] - cur) * secondDerivScale;
          Dxy = ((midImg[2][2] - midImg[2][0]) - (midImg[0][2]-midImg[0][0])) * secondDerivScale;
          Dxs = ((upperImg[1][2] - upperImg[1][0]) - (lowerImg[1][2] - lowerImg[1][0])) * crossDerivScale;
          Dys = ((upperImg[2][1] - upperImg[0][1]) - (lowerImg[2][1] - lowerImg[0][1])) * crossDerivScale;  

          // get x^, y^, s^
          xhatX = -1*Dx*(Dxx+Dxy+Dss);
          xhatY = -1*Dy*(Dxy+Dyy+Dys);
          xhatS = -1*Ds*(Dxs+Dys+Dss);
          
          isKeyPoint = true;              
          if(fabs(xhatX) < 0.0003 && fabs(xhatY) < 0.0003 && fabs(xhatS) < 0.00002){
            break;
          }else{      
            if(fabs(xhatX) > (float)(INT_MAX/3) || fabs(xhatY) > (float)(INT_MAX/3) || fabs((float)xhatS) > (float)(INT_MAX/3)){
              isKeyPoint = false;
              break;
            }
            
            offsetX = offsetX + (int)round(xhatX);
            offsetY = offsetY + (int)round(xhatY);
            // approximation for offsetS 
            offsetS = offsetS + 2.4 * (int)round(xhatS);
          
            if(x+offsetX < 8 || x+offsetX >= _cols-9 || y+offsetY < 8 || y+offsetY > _rows-9){
              isKeyPoint = false;
              break;
            }
          }
        }
        
        if(isKeyPoint){
          nDx = (midImg[1][2] - midImg[1][0]) * derivScale;
          nDy = (midImg[2][1] - midImg[0][1]) * derivScale;
          nDs = (upperImg[1][1] - lowerImg[1][1]) * derivScale;
          
          float temp = fabs(nDx * xhatX) + fabs(nDy * xhatY) + fabs(nDs * xhatS);
          contrast = (midImg[1][1]*imgScale) + (0.5*temp);
          
          // reject the low-contrast points with approximation
          // eliminate the edege responses by using Hessian matrix 
          // point at (offsetX, offsetY)
          if(contrast > 0.06){
            // hessian matrix
            nCur = midImg[1][1]*2;
            nDxx = (midImg[1][2] + midImg[1][0] - nCur) * secondDerivScale;
            nDyy = (midImg[2][1] - midImg[0][1] - nCur) * secondDerivScale;
            nDxy = ((midImg[2][2] - midImg[2][0]) - (midImg[0][2]-midImg[0][0])) * secondDerivScale;
            trace = nDxx + nDyy;
            det = (nDxx*nDyy) - (nDxy*nDxy);
            
            if(trace*trace < fabs(det)*100){
              _keypoints->x[numOfKeys] = x + offsetX;
              _keypoints->y[numOfKeys] = y + offsetY;
              numOfKeys++;
            }
          }
        }
      }
      
      // 1 shift
      for(int i = _cols*2+2 ; i > 0; i--){
        upperBuf[i] = upperBuf[i-1];
        midBuf[i] = midBuf[i-1];
        lowerBuf[i] = lowerBuf[i-1];
      } 
      upperBuf[0] = _upperLayer[(y+1)*_cols + (x+3)];
      midBuf[0] = _midLayer[(y+1)*_cols + (x+3)];
      lowerBuf[0] = _lowerLayer[(y+1)*_cols + (x+3)];
    }
  }
  _keypoints->numOfKeys = numOfKeys;
}
