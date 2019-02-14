/************************************

  Author : Jin Heo
  Date : 2017. 11. 30

************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

vx_status VX_CALLBACK VxKeyDescriptorKernel(vx_node node,
    const vx_reference *parameters, vx_uint32 num)
{
  vx_status status = VX_ERROR_INVALID_PARAMETERS;
  if (num == 6)
  {
    // input image, sigma for magnitude gaussian, input keypoint
    vx_image srcImg = (vx_image)parameters[0];
    vx_scalar srcSig = (vx_scalar)parameters[1];
    vx_array srcKey = (vx_array)parameters[2];

    // output key
    vx_array destKey = (vx_array)parameters[3];
    vxTruncateArray(destKey, 0);

    // output descriptor Dir, Mag
    vx_array destDescDir = (vx_array)parameters[4];
    vx_array destDescMag = (vx_array)parameters[5];
    vxTruncateArray(destDescDir, 0);
    vxTruncateArray(destDescMag, 0);

    void *srcImgBase;
    vx_keypoint_t *srcKeyBase;
    vx_keypoint_t srcKeypoint;
    vx_keypoint_t destKeypoint;
    vx_keypoint_t addKeypoint;

    vx_size numOfKeys = 0;
    vx_size keyStride = sizeof(vx_keypoint_t);
    vx_float32 sigma;

    vx_map_id srcImgID;
    vx_map_id srcKeyID;
    vx_imagepatch_addressing_t srcAddr;
    vx_rectangle_t validRect;

    // input param..
    // image
    vxGetValidRegionImage(srcImg, &validRect);
    vxMapImagePatch(srcImg, &validRect, 0, &srcImgID, &srcAddr, &srcImgBase,
        VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);

    // key array
    vxQueryArray(srcKey, VX_ARRAY_NUMITEMS, &numOfKeys, sizeof(numOfKeys));
    vxMapArrayRange(srcKey, 0, numOfKeys, &srcKeyID, &keyStride,
        (void **)&srcKeyBase, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);

    // sigma
    vxCopyScalar(srcSig, (void*)&sigma, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);

    vx_uint8 *pixel;
    vx_uint8 buf[18][18];
    vx_float32 mag[16][16];
    vx_float32 theta[16][16];
    vx_float32 gaussianMatrix[16][16];
    vx_float32 gaussianSum = 0;

    vx_float32 degreeConv = 180/3.1416;
    vx_int32 degree;

    vx_float32 bins36[36];
    vx_float32 bins8[8];
    vx_int32 binIndex;

    vx_int32 cX, cY;
    vx_int32 kX, kY;

    //printf("num of keys : %d \r\n", numOfKeys);
    // gaussian blur for magnitude
    for(vx_uint32 y = 0 ; y < 16 ; y++){
      for(vx_uint32 x = 0 ; x < 16 ; x++){
        gaussianMatrix[y][x] = 0;
        vx_uint32 distX = abs(x - 8);
        vx_uint32 distY = abs(y - 8);
        gaussianMatrix[y][x] = (1 / (2 * 3.14159*sigma*sigma))
          / exp( ( (distX*distX + distY*distY) / (2 * sigma*sigma) ) );
        gaussianSum += gaussianMatrix[y][x];
      }
    }
    for(vx_uint32 y = 0 ; y < 16; y++)
      for(vx_uint32 x = 0 ; x < 16 ; x++)
        gaussianMatrix[y][x] /= gaussianSum;

    // each key, each description...
    for(vx_size i = 0 ; i < numOfKeys ; i++){

      for(vx_uint32 y = 0 ; y < 16 ; y++){
        for(vx_uint32 x = 0 ; x < 16 ; x++){
          mag[y][x] = 0;
          theta[y][x] = 0;
        }
      }
      for(vx_uint32 j = 0 ; j < 36 ; j++) bins36[j] = 0;

      // read keypoint direction...
      srcKeypoint = vxArrayItem(vx_keypoint_t, srcKeyBase, i, keyStride);
      kX = srcKeypoint.x;
      kY = srcKeypoint.y;

      if( kX > 10 && kX < srcAddr.dim_x-12 && kY > 10 &&
          kY < srcAddr.dim_y-12 ){ // bolder regulation...
        for(vx_int32 y = -8 ; y < 10 ; y++){
          for(vx_int32 x = -8 ; x < 10 ; x++){
            pixel =
              vxFormatImagePatchAddress2d(srcImgBase, kX+x, kY+y, &srcAddr);
            buf[y+8][x+8] = *pixel;
          }
        }
        // mag & theta
        for(vx_uint32 y = 1 ; y < 17 ; y++){
          for(vx_uint32 x = 1 ; x < 17 ; x++){
            cX = buf[y][x+1] - buf[y][x-1];
            cY = buf[y+1][x] - buf[y-1][x];
            mag[y-1][x-1] = sqrt(cX*cX + cY*cY);
            theta[y-1][x-1] = atan2(cY, cX);
          }
        }

        for(vx_uint32 y = 0 ; y < 16; y++)
          for(vx_uint32 x = 0 ; x < 16 ; x++)
            mag[y][x] *= gaussianMatrix[y][x];

        // key orientation...
        for(vx_uint32 y = 0 ; y < 16; y++){
          for(vx_uint32 x = 0 ; x < 16 ; x++){
            degree = (vx_int32)(degreeConv * theta[y][x]);
            if(degree < 0) degree+=359;
            //printf("%f \r\n", degree);

            binIndex = (vx_int32)(degree / 10);
            bins36[binIndex] += mag[y][x];
          }
        }
        vx_float32 maxMag36 = bins36[0];
        for(vx_uint32 k = 1 ; k < 36; k++)
          if(bins36[k] > maxMag36) maxMag36 = bins36[k];

        for(vx_uint32 k = 0 ; k < 36; k++){
          if(bins36[k] == maxMag36){
            destKeypoint.x = kX;
            destKeypoint.y = kY;
            destKeypoint.orientation = k+1;
            vxAddArrayItems(destKey, 1, &destKeypoint, sizeof(vx_keypoint_t));
          }
        }

        // 16 Descriptor Vectors...
        vx_uint32 idxY, idxX;
        vx_uint32 gY, gX;
        vx_uint8 tempDir[16];
        vx_uint8 tempMag[16];

        for(vx_uint32 k = 0 ; k < 16 ; k++){
          idxY = k / 4;
          idxX = k % 4;
          for(vx_uint32 l = 0 ; l < 8 ; l++) bins8[l] = 0;

          for(vx_uint32 y = 0 ; y < 4; y++){
            for(vx_uint32 x = 0 ; x < 4; x++){
              gY = y + (idxY*4);
              gX = x + (idxX*4);
              degree = abs(degreeConv * theta[gY][gX]);
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

          vx_float32 maxMag8 = bins8[0];
          for(vx_uint32 l = 1 ; l < 8; l++)
            if(maxMag8 < bins8[l]) maxMag8 = bins8[l];
          for(vx_uint32 l = 0 ; l < 8; l++){
            if(maxMag8 == bins8[l]){
              // Save Keypoint Descriptor
              tempDir[k] = l+1;
              if( (maxMag8/maxMag36) > 1 ) tempMag[k] = 255;
              else tempMag[k] = (maxMag8/maxMag36) * 255;
              vxAddArrayItems(destDescDir, 1, &tempDir[k], sizeof(vx_uint8));
              vxAddArrayItems(destDescMag, 1, &tempMag[k], sizeof(vx_uint8));
            }
          }
        }
        // Additional Keypoint...
        for(vx_uint32 k = 0 ; k < 36; k++){
          bins36[k] = bins36[k] / maxMag36;
          if(bins36[k] >= 0.8 && bins36[k] < 1){
            addKeypoint.x = destKeypoint.x;
            addKeypoint.y = destKeypoint.y;
            addKeypoint.orientation = k+1;

            vxAddArrayItems(destKey, 1, &addKeypoint, sizeof(vx_keypoint_t));
            vxAddArrayItems(destDescDir, 16, tempDir, sizeof(vx_uint8));
            vxAddArrayItems(destDescMag, 16, tempMag, sizeof(vx_uint8));
          }
        }

      }
    }
    vxUnmapArrayRange(srcKey, srcKeyID);
    vxUnmapImagePatch(srcImg, srcImgID);

    status = VX_SUCCESS;
  }
  return status;
}
