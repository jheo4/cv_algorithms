/************************************

  Author : Jin Heo
  Date : 2017. 11. 30

************************************/
#include <stdio.h>
#include <math.h>
vx_status VX_CALLBACK VxKeypointKernel(vx_node node,
    const vx_reference *parameters, vx_uint32 num)
{
  vx_status status = VX_ERROR_INVALID_PARAMETERS;
  if (num == 11)
  {
    // 0 ~ 3 : dog
    // 4 ~ 8 : gaussian img
    // 9 ~ 10 : output keypoint array
    vx_image srcDog[4];
    for(vx_uint32 i = 0 ; i < 4 ; i++)
      srcDog[i] = (vx_image)parameters[i];

    vx_image srcImg[5];
    for(vx_uint32 i = 4 ; i < 9 ; i++)
      srcImg[i-4] = (vx_image)parameters[i];

    vx_array destArray[2];
    for(vx_uint32 i = 9 ; i < 11 ; i++){
      destArray[i-9] = (vx_array)parameters[i];
      vxTruncateArray(destArray[i-9], 0);
    }

    void *srcDogBase[4];
    void *srcImgBase[5];
    vx_keypoint_t keypoint;

    vx_map_id srcDogID[4];
    vx_map_id srcImgID[5];

    vx_imagepatch_addressing_t srcAddr;
    vx_imagepatch_addressing_t destAddr;
    vx_rectangle_t validRect;

    vx_uint8 upBox[3][3];
    vx_uint8 midBox[3][3];
    vx_uint8 downBox[3][3];

    vx_uint8 *upPixel, *midPixel, *downPixel, *compPixel;
    vx_uint8 *destPixel;

    vx_bool isCandidates = vx_false_e;

    vx_float32 imgScale = 0.00392; // 1/255
    vx_float32 derivScale = 0.00392; // 1/510
    vx_float32 secondDerivScale = 0.00196;   // 1/255
    vx_float32 crossDerivScale = 0.00392; // 1/1020
    //vx_float32 derivScale = 0.00196; // 1/510
    //vx_float32 secondDerivScale = 0.00392;   // 1/255
    //vx_float32 crossDerivScale = 0.00098; // 1/1020

    vx_float32 Dx=0, Dy=0, Ds=0;
    vx_float32 Dxx=0, Dyy=0, Dss=0, Dxy=0, Dxs=0, Dys=0;
    vx_float32 cur=0;
    vx_float32 xhatX=0, xhatY=0, xhatS=0;

    vx_float32 nDx=0, nDy=0, nDs=0;
    vx_float32 nDxx=0, nDxy=0, nDyy=0;
    vx_float32 nCur=0;
    vx_float32 trace=0, det=0;
    vx_int32 offsetX, offsetY, offsetS;

    vxGetValidRegionImage(srcDog[0], &validRect);

    for(vx_uint32 i = 0 ; i < 4 ; i++)
      vxMapImagePatch(srcDog[i], &validRect, 0, &srcDogID[i], &srcAddr,
          &srcDogBase[i], VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);

    for(vx_uint32 i = 0 ; i < 5 ; i++)
      vxMapImagePatch(srcImg[i], &validRect, 0, &srcImgID[i], &srcAddr,
          &srcImgBase[i], VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);

    for(vx_uint32 i = 1; i <= 2; i++){

      for(vx_uint32 y = 1 ; y < (srcAddr.dim_y-1) ; y++){
        for(vx_uint32 x = 1 ; x < (srcAddr.dim_x-1) ; x++){
          compPixel =
            vxFormatImagePatchAddress2d(srcDogBase[i], x, y, &srcAddr);

          for(vx_uint32 bY = 0 ; bY < 3 ; bY++ ){
            for(vx_uint32 bX = 0 ; bX < 3 ; bX++ ){
              upPixel = vxFormatImagePatchAddress2d(srcDogBase[i-1], x-1+bX,
                            y-1+bY, &srcAddr);
              midPixel = vxFormatImagePatchAddress2d(srcDogBase[i], x-1+bX,
                            y-1+bY, &srcAddr);
              downPixel = vxFormatImagePatchAddress2d(srcDogBase[i+1], x-1+bX,
                            y-1+bY, &srcAddr);
              upBox[bY][bX] = *upPixel;
              downBox[bY][bX] = *downPixel;
              midBox[bY][bX] = *midPixel;
            }
          }
          if(*compPixel > upBox[0][0] && *compPixel > upBox[0][1] &&
              *compPixel > upBox[0][2] && *compPixel > upBox[1][0] &&
              *compPixel > upBox[1][1] && *compPixel > upBox[1][2] &&
              *compPixel > upBox[2][0] && *compPixel > upBox[2][1] &&
              *compPixel > upBox[2][2] &&*compPixel > downBox[0][0] &&
              *compPixel > downBox[0][1] && *compPixel > downBox[0][2] &&
              *compPixel > downBox[1][0] && *compPixel > downBox[1][1] &&
              *compPixel > downBox[1][2] && *compPixel > downBox[2][0] &&
              *compPixel > downBox[2][1] && *compPixel > downBox[2][2] &&
              *compPixel > midBox[0][0] && *compPixel > midBox[0][1] &&
              *compPixel > midBox[0][2] && *compPixel > midBox[1][0] &&
              *compPixel > midBox[1][2] && *compPixel > midBox[2][0] &&
              *compPixel > midBox[2][1] && *compPixel > midBox[2][2]){
            isCandidates = vx_true_e;
          }

          if(*compPixel < upBox[0][0] && *compPixel < upBox[0][1] &&
            *compPixel < upBox[0][2] && *compPixel < upBox[1][0] &&
            *compPixel < upBox[1][1] && *compPixel < upBox[1][2] &&
            *compPixel < upBox[2][0] && *compPixel < upBox[2][1] &&
            *compPixel < upBox[2][2] &&*compPixel < downBox[0][0] &&
            *compPixel < downBox[0][1] && *compPixel < downBox[0][2] &&
            *compPixel < downBox[1][0] && *compPixel < downBox[1][1] &&
            *compPixel < downBox[1][2] && *compPixel < downBox[2][0] &&
            *compPixel < downBox[2][1] && *compPixel < downBox[2][2] &&
            *compPixel < midBox[0][0] && *compPixel < midBox[0][1] &&
            *compPixel < midBox[0][2] && *compPixel < midBox[1][0] &&
            *compPixel < midBox[1][2] && *compPixel < midBox[2][0] &&
            *compPixel < midBox[2][1] && *compPixel < midBox[2][2]){
            isCandidates = vx_true_e;
          }

          if(isCandidates){
            vx_uint32 hc = 0;
            vx_bool finalKeyPoint;
            offsetX = 0;
            offsetY = 0;
            offsetS = 0;

            for(hc = 0 ; hc < 3 ; hc++){

              for(vx_uint32 bY = 0 ; bY < 3 ; bY++ ){
                for(vx_uint32 bX = 0 ; bX < 3 ; bX++ ){
                  if(i == 1){
                    upPixel = vxFormatImagePatchAddress2d(srcImgBase[0],
                                  x-1+bX+offsetX, y-1+bY+offsetY, &srcAddr);
                    midPixel = vxFormatImagePatchAddress2d(srcImgBase[1],
                                  x-1+bX+offsetX, y-1+bY+offsetY, &srcAddr);
                    downPixel = vxFormatImagePatchAddress2d(srcImgBase[2],
                                  x-1+bX+offsetX, y-1+bY+offsetY, &srcAddr);
                  }
                  else{
                    upPixel = vxFormatImagePatchAddress2d(srcImgBase[2],
                                  x-1+bX+offsetX, y-1+bY+offsetY, &srcAddr);
                    midPixel = vxFormatImagePatchAddress2d(srcImgBase[3],
                                  x-1+bX+offsetX, y-1+bY+offsetY, &srcAddr);
                    downPixel = vxFormatImagePatchAddress2d(srcImgBase[4],
                                  x-1+bX+offsetX, y-1+bY+offsetY, &srcAddr);
                  }
                  upBox[bY][bX] = *upPixel;
                  downBox[bY][bX] = *downPixel;
                  midBox[bY][bX] = *midPixel;
                }
              }

              Dx = (midBox[1][2] - midBox[1][0]) * derivScale;
              Dy = (midBox[2][1] - midBox[0][1]) * derivScale;
              Ds = (upBox[1][1] - downBox[1][1]) * derivScale;
              cur = midBox[1][1] * 2;

              Dxx = (midBox[1][2] + midBox[1][0] - cur) * secondDerivScale;
              Dyy = (midBox[2][1] + midBox[0][1] - cur) * secondDerivScale;
              Dss = (upBox[1][1] + downBox[1][1] - cur) * secondDerivScale;
              Dxy = ((midBox[2][2] - midBox[2][0]) -
                  (midBox[0][2]-midBox[0][0])) * secondDerivScale;
              Dxs = ((upBox[1][2] - upBox[1][0]) -
                  (downBox[1][2] - downBox[1][0])) * crossDerivScale;
              Dys = ((upBox[2][1] - upBox[0][1]) -
                  (downBox[2][1] - downBox[0][1])) * crossDerivScale;

              xhatX = -1*Dx*(Dxx+Dxy+Dss);
              xhatY = -1*Dy*(Dxy+Dyy+Dys);
              xhatS = -1*Ds*(Dxs+Dys+Dss);

              finalKeyPoint = vx_true_e;
              if(fabs(xhatX) < 0.5 && fabs(xhatY) < 0.5 && fabs(xhatS) < 0.5){
                break;
              }else{
                if(fabs(xhatX) > (float)(2147483647/3) ||
                    fabs(xhatY) > (float)(2147483647/3) ||
                    fabs((float)xhatS) > (float)(2147483647/3)){
                  finalKeyPoint = vx_false_e;
                  break;
                }
                offsetX = offsetX + (float)round(xhatX);
                offsetY = offsetY + (float)round(xhatY);
                offsetS = 2.4 * (float)round(xhatS);

                if(offsetX < 8 || offsetX >= srcAddr.dim_x-9 ||
                    offsetY < 8 || offsetY > srcAddr.dim_y-9){
                  finalKeyPoint = vx_false_e;
                  break;
                }
              }
            }
            if(finalKeyPoint){
              nDx = (midBox[1][2] - midBox[1][0]) * derivScale;
              nDy = (midBox[2][1] - midBox[0][1]) * derivScale;
              nDs = (upBox[1][1] - downBox[1][1]) * derivScale;

              vx_float32 temp =
                fabs(nDx * xhatX) + fabs(nDy * xhatY) + fabs(nDs * xhatS);

              vx_float32 contrast = (midBox[1][1]*imgScale) + (0.5*temp);
              //printf("contrast : %f \r\n", contrast);
              if(contrast > 0.0025){
                nCur = midBox[1][1]*2;
                nDxx = (midBox[1][2] + midBox[1][0] - nCur) * secondDerivScale;
                nDyy = (midBox[2][1] - midBox[0][1] - nCur) * secondDerivScale;
                nDxy = ((midBox[2][2] - midBox[2][0]) -
                            (midBox[0][2]-midBox[0][0])) * secondDerivScale;
                trace = nDxx + nDyy;
                det = (nDxx*nDyy) - (nDxy*nDxy);
                if(trace*trace < fabs(det)*100){
                  keypoint.x = x;
                  keypoint.y = y;
                  vxAddArrayItems(destArray[i-1], 1, &keypoint,
                      sizeof(vx_keypoint_t));
                }
              }
            }
          }
          isCandidates = vx_false_e;
        }
      }

    }
    vx_size tempSize[2];
    vxQueryArray(destArray[0], VX_ARRAY_NUMITEMS, &tempSize[0],
        sizeof(vx_size));
    vxQueryArray(destArray[1], VX_ARRAY_NUMITEMS, &tempSize[1],
        sizeof(vx_size));
    printf("0 : %d \r\n", tempSize[0]);
    printf("1 : %d \r\n", tempSize[1]);

    for(vx_uint32 i = 0 ; i < 4; i++)
      vxUnmapImagePatch(srcDog[i], srcDogID[i]);
    for(vx_uint32 i = 0 ; i < 5; i++)
      vxUnmapImagePatch(srcImg[i], srcImgID[i]);
    status = VX_SUCCESS;
  }
  return status;

}
