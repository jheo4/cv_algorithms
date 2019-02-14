/************************************

  Author : Jin Heo
  Date : 2017. 11. 27

************************************/
vx_status VX_CALLBACK VxDogKernel(vx_node node, const vx_reference *parameters,
    vx_uint32 num)
{
  vx_status status = VX_ERROR_INVALID_PARAMETERS;
  if (num == 9)
  {
    // input Gaussian blurred images
    vx_image src[5];
    for(vx_uint32 i = 0 ; i < 5; i ++){
      src[i] = (vx_image)parameters[i];
    }

    // output DoG images
    vx_image dest[4];
    for(vx_uint32 i = 5 ; i < 9; i ++){
      dest[i-5] = (vx_image)parameters[i];
    }

    void *srcBase[5];
    void *destBase[4];

    vx_imagepatch_addressing_t srcAddr;
    vx_imagepatch_addressing_t destAddr;
    vx_rectangle_t validRect;

    vx_map_id srcID[5];
    vx_map_id destID[4];

    vx_uint8 *srcPixel1;
    vx_uint8 *srcPixel2;
    vx_int32 result;
    vx_uint8 *destPixel;

    // for access to the images, map the image
    vxGetValidRegionImage(src[0], &validRect);
    for(vx_uint32 i = 0 ; i < 5; i++){
      vxMapImagePatch(src[i], &validRect, 0, &srcID[i], &srcAddr, &srcBase[i],
          VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);
    }
    for(vx_uint32 i = 0 ; i < 4; i++){
      vxMapImagePatch(dest[i], &validRect, 0, &destID[i], &destAddr,
          &destBase[i], VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0);
    }

    for(vx_uint32 i = 0 ; i < 4; i++){
      for(vx_uint32 y = 0 ; y < srcAddr.dim_y ; y++){
        for(vx_uint32 x = 0 ; x < srcAddr.dim_x ; x++){
          srcPixel1 =
            vxFormatImagePatchAddress2d(srcBase[i], x, y, &srcAddr);
          srcPixel2 =
            vxFormatImagePatchAddress2d(srcBase[i+1], x, y, &srcAddr);
          destPixel =
            vxFormatImagePatchAddress2d(destBase[i], x, y, &destAddr);
          // output clamping
          result = *srcPixel1 - *srcPixel2;
          if(result < 0)
            *destPixel = 0;
          else if(result > 255)
            *destPixel = 255;
          else {
            *destPixel = (vx_uint8)(result*3);
          }
        }
      }
    }

    for(vx_uint32 i = 0 ; i < 5; i++)
      vxUnmapImagePatch(src[i], srcID[i]);
    for(vx_uint32 i = 0 ; i < 4; i++)
      vxUnmapImagePatch(dest[i], destID[i]);
    status = VX_SUCCESS;
  }
  return status;
}
