//
//  Author : Jin Heo
//  Date : Sep, 27, 2017
//

#include "../host/inc/defines.h"
__kernel void cl_stereoMatching(
    global uchar * restrict _imgL,
    global uchar * restrict _imgR,
    global uchar * restrict _res)
{
  // Iteration : PADSIZE ~ PAD_ROWS and COLS
  uint mX = PADSIZE;
  uint mY = PADSIZE;

  uchar mLeftBox[WNDSIZE * WNDSIZE];
  uchar mRightBox[WNDSIZE * WNDSIZE];
  uchar mRightArea[WNDSIZE * WNDRANGE];

  int result[RES_RANGE] = {0,};
  int sum, sad, offset=0;
  int x, y;
  int idx=0;

  for(int i = 0 ; i < WNDSIZE ; i++){
    for(int j = 0 ; j < WNDSIZE ; j++){
      mLeftBox[(i*WNDSIZE) + j] = _imgL[i*COLS + j];
    }
  }
  for(int i = 0 ; i < WNDSIZE ; i++){
    for(int j = 0 ; j < WNDRANGE ; j++){
      mRightArea[(i*WNDRANGE) + j] = _imgR[i*COLS + j];
    }
  }

  while(mY < PAD_ROWS){
    while(mX < PAD_COLS){
      idx = 0;

      for(int i = offset ; i < RES_RANGE ; i++){
        for(y = 0 ; y < WNDSIZE ; y++){
          for(x = 0 ; x < WNDSIZE ; x++){
            mRightBox[y*WNDSIZE+x] = mRightArea[y*WNDRANGE + x+i];
          }
        }

        for(int j = 0 ; j < WNDSIZE * WNDSIZE ; j++)
                sum+=abs(mLeftBox[j] - mRightBox[j]);
        result[idx] = sum;
        sum = 0;
        idx++;
      }

      sad = result[0];
      uchar distance;

      for(int i = 1 ; i < RES_RANGE ; i++)
        if(sad > result[i] && result[i]!=0){
          sad = result[i];
          distance = i;
        }
      // Save result
      _res[ (mY-PADSIZE)*RES_COLS + mX-PADSIZE] = distance*15;

      for(int i = 0 ; i < RES_RANGE; i++) result[i] = 0;
      idx = 0;
      mX++;

      // move mLeftBox to X direction
      for(y = 0 ; y < WNDSIZE ; y++){
        for(x = 0 ; x < WNDSIZE-1 ; x++){
          mLeftBox[y*WNDSIZE + x] = mLeftBox[y*WNDSIZE + x+1];
        }
        mLeftBox[y*WNDSIZE + x] = _imgL[(mY-PADSIZE+y)*COLS + mX-PADSIZE+x];
      }

      // move mRightArea to X direction
      if(mX+WNDRANGE < PAD_COLS){
        for(y = 0 ; y < WNDSIZE ; y++){
          for(x = 0 ; x < WNDRANGE-1; x++){
            mRightArea[(y*WNDRANGE) + x] = mRightArea[(y*WNDRANGE) + x+1];
          }
          mRightArea[(y*WNDRANGE) + x] = _imgR[(mY-PADSIZE+y)*COLS
                                               + mX-PADSIZE+x];
        }
      }else{
        offset++;
      }
    }

    offset = 0;
    mX = PADSIZE;
    mY++;

    for(y = 0 ; y < WNDSIZE ; y++){
      for(x = 0; x < WNDSIZE ; x++){
        mLeftBox[(y*WNDSIZE) + x] = _imgL[ ((mY-PADSIZE+y)*COLS) + x];
      }
    }

    for(y = 0 ; y < WNDSIZE ; y++){
      for(x = 0; x < WNDRANGE ; x++){
        mRightArea[(y*WNDRANGE) + x] = _imgR[(mY-PADSIZE+y)*COLS + x];
      }
    }
  }
}
