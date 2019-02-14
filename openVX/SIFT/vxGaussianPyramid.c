/************************************

  Author : Jin Heo
  Date : 2017. 11. 22

************************************/
#define KERSIZE 5
#define KERRAD ((KERSIZE-1)/2)
vx_status VX_CALLBACK VxGaussianPyramidKernel(vx_node node,
    const vx_reference *parameters, vx_uint32 num)
{
  vx_status status = VX_ERROR_INVALID_PARAMETERS;
  if (num == 7)
  {
    // source image
    vx_image src = (vx_image)parameters[0];

    // output gaussian blur image
    vx_image dest[5];
    for(vx_uint32 i = 0 ; i < 5; i++)
      dest[i] = (vx_image)parameters[i+2];

    vx_scalar kScalar = (vx_scalar)parameters[1];
    vx_float32 kVal;
    vxCopyScalar(kScalar, (void *)&kVal, VX_READ_ONLY, VX_MEMORY_TYPE_HOST);
    void *srcBase;
    void *destBase[5];

    vx_imagepatch_addressing_t srcAddr;
    vx_imagepatch_addressing_t destAddr;
    vx_rectangle_t validRect;

    vx_map_id srcID;
    vx_map_id destID[5];

    vxGetValidRegionImage(src, &validRect);
    vxMapImagePatch(src, &validRect, 0, &srcID, &srcAddr, &srcBase,
        VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);
    for(vx_uint32 i = 0 ; i < 5; i++)
      vxMapImagePatch(dest[i], &validRect, 0, &destID[i], &destAddr,
          &destBase[i], VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0);

    vx_float32 gaussianMatrix[KERSIZE][KERSIZE];
    vx_float32 gaussianSum=0;
    vx_float32 destSum=0;
    vx_float32 sigma;

    vx_uint8 *srcPixel;
    vx_uint8 *destPixel;

    for(vx_uint32 i = 0 ; i < 5 ; i++){
      // sigma setting
      switch(i){
        case 0:
          sigma = 1*kVal;
        break;
        case 1:
          sigma = sqrt(2)*kVal;
        break;
        case 2:
          sigma = 2*kVal;
        break;
        case 3:
          sigma = 2*sqrt(2)*kVal;
        break;
        case 4:
          sigma = 4*kVal;
        break;
      }

      // gaussian matrix setting
      gaussianSum=0;
      for(vx_uint32 y = 0 ; y < KERSIZE; y++){
        for(vx_uint32 x = 0 ; x < KERSIZE ; x++){
          vx_uint32 distX = abs(x - KERRAD);
          vx_uint32 distY = abs(y - KERRAD);
          gaussianMatrix[y][x] = (1 / (2 * 3.14159*sigma*sigma))
            / exp( ( (distX*distX + distY*distY) / (2 * sigma*sigma) ) );
          gaussianSum += gaussianMatrix[y][x];
        }
      }
      for(vx_uint32 y = 0 ; y < KERSIZE; y++){
        for(vx_uint32 x = 0 ; x < KERSIZE ; x++){
          gaussianMatrix[y][x] /= gaussianSum;
        }
      }

      // multiply gaussian matrix with image pixels
      for(vx_uint32 y = KERRAD ; y < (srcAddr.dim_y-KERRAD) ; y++){
        for(vx_uint32 x = KERRAD ; x < (srcAddr.dim_x-KERRAD) ; x++){

          destSum = 0;
          for(vx_uint32 i = 0; i < KERSIZE ; i++){
            for(vx_uint32 j = 0 ; j < KERSIZE ; j++){
              srcPixel = vxFormatImagePatchAddress2d(srcBase, (x+j-KERRAD),
                  (y+i-KERRAD), &srcAddr);
              destSum += ((*srcPixel) * gaussianMatrix[i][j]);
            }
          }

          destPixel =
            vxFormatImagePatchAddress2d(destBase[i], x, y, &destAddr);
          if(destSum < 0)
            *destPixel = 0;
          else if(destSum > UINT8_MAX)
            *destPixel = UINT8_MAX;
          else
            *destPixel = (vx_uint8)destSum;
        }
      }
    }

    vxUnmapImagePatch(src, srcID);
    for(vx_uint32 i = 0 ; i < 5; i++)
      vxUnmapImagePatch(dest[i], destID[i]);

    status = VX_SUCCESS;
  }
  return status;
}
