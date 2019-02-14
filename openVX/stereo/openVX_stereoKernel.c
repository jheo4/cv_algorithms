/***************************************
    Author : Jin Heo
    Date : 2017. 11. 21
***************************************/

#include <VX/vx.h>
#include <stdio.h>
#include <stdlib.h>
#include "user_kernel_name.h"
#include "user_kernel_stubs.h"
#include <stdio.h>
#include <stdlib.h>
#define WNDSIZE 7
#define PADSIZE ((WNDSIZE-1)/2)
#define WNDRANGE 40
#define SCALE (255/WNDRANGE)
vx_status VX_CALLBACK VxStereoKernel(vx_node node, const vx_reference *parameters, vx_uint32 num)
{
    vx_status status = VX_ERROR_INVALID_PARAMETERS;
    if (num == 3)
  {
    vx_image srcL = (vx_image)parameters[0];
    vx_image srcR = (vx_image)parameters[1];
    vx_image dest = (vx_image)parameters[2];

    void *srcLbase = NULL;
    void *srcRbase = NULL;
    void *destBase = NULL;
    vx_imagepatch_addressing_t srcLaddr;//= VX_IMAGEPATCH_ADDR_INIT;
    vx_imagepatch_addressing_t srcRaddr;
    vx_imagepatch_addressing_t destAddr;
    vx_rectangle_t validRectL;
    vx_rectangle_t validRectR;
    
    vx_map_id srcLid;
    vx_map_id srcRid;
    vx_map_id destMapID;

    vxGetValidRegionImage(srcL, &validRectL);
    vxGetValidRegionImage(srcR, &validRectR);

    vxMapImagePatch(srcL, &validRectL, 0, &srcLid, &srcLaddr, &srcLbase, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);
    vxMapImagePatch(srcR, &validRectR, 0, &srcRid, &srcRaddr, &srcRbase, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);
    vxMapImagePatch(dest, &validRectL, 0, &destMapID, &destAddr, &destBase, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0);

    vx_int32 sX = PADSIZE;
    vx_int32 sY = PADSIZE;
    vx_int32 eX = srcLaddr.dim_x - PADSIZE;
    vx_int32 eY = srcLaddr.dim_y - PADSIZE;

    vx_int8 lBox[WNDSIZE][WNDSIZE];
    vx_int8 rBox[WNDSIZE][WNDSIZE];

    vx_int32 rSumList[WNDRANGE];
    vx_int32 sad, dist;
    vx_uint8 *lPixel;
    vx_uint8 *rPixel;

    for(vx_uint32 y = sY ; y < eY ; y++){
      for(vx_uint32 x = sX; x < eX ; x++){

        for(vx_uint32 i = y-PADSIZE; i <= y+PADSIZE; i++){
          for(vx_uint32 j = x-PADSIZE; j <= x+PADSIZE; j++){
            lPixel = vxFormatImagePatchAddress2d(srcLbase, j, i, &srcLaddr);
            lBox[i-(y-PADSIZE)][j-(x-PADSIZE)] = *lPixel;
          }
        }

        for(vx_uint32 i = 0 ; i < WNDRANGE; i++){
          if(x+i < eX){
            rSumList[i] =0;            
            for(vx_uint32 j = y-PADSIZE; j <= y+PADSIZE; j++){
              for(vx_uint32 k = x+i-PADSIZE; k <= x+i+PADSIZE; k++){
                rPixel = vxFormatImagePatchAddress2d(srcRbase, k, j, &srcRaddr);
                rBox[j-(y-PADSIZE)][k-(x+i-PADSIZE)] = *rPixel;
              }
            }   
            for(vx_uint32 j = 0 ; j < WNDSIZE ; j++)  
              for(vx_uint32 k = 0 ; k < WNDSIZE ; k++)
                rSumList[i] += abs(rBox[j][k]-lBox[j][k]);            
          }
          else{
            rSumList[i] = -1;
          }
        }

        sad = rSumList[0];
        dist = 0;
        for(vx_uint32 i = 1 ; i < WNDRANGE; i++){
          if(rSumList[i] >= 0 && rSumList[i] < sad){
            sad = rSumList[i];
            dist = i;
          }
        }

        vx_uint8 *destPixel = vxFormatImagePatchAddress2d(destBase, x, y, &destAddr);
        *destPixel = (vx_uint8)(dist*SCALE);
      }
    }
    
    vxUnmapImagePatch(srcL, srcLid);
    vxUnmapImagePatch(srcR, srcRid);
    status = VX_SUCCESS;
  }
  return status;
}