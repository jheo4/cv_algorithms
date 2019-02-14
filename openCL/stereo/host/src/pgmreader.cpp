#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include "pgmreader.h"
#include "defines.h"


void pgmReader(const char *filename, unsigned char *calc_data,
    unsigned int *disp_data){
  FILE *fp = NULL;
  fp = fopen(filename, "rb");
  if(fp ==NULL){
    printf("fp error! \r\n");
    fclose(fp);
    return;
  }
  const size_t headerSize = 0x40;
  char header[headerSize];

  // remove the header
  fgets(header, headerSize, fp);
  printf("%s", header);
  fgets(header, headerSize, fp);
  printf("%s", header);
  fgets(header, 0x20, fp);
  printf("%s", header);

  unsigned char *raw = (unsigned char *)malloc(sizeof(unsigned char) *
      ROWS * COLS);
  fread(raw, sizeof(unsigned char), ROWS*COLS, fp);

  // transferring data from FILE to calcArr and dispArr...
  for (int i = 0 ; i != ROWS*COLS ; i++) {
    calc_data[i] = raw[i];
    calcToDisp(disp_data, calc_data, i);
  }
  free(raw);
  fclose(fp);
  printf("ok!\r\n");
}


void pgmWriter(const char *filename, unsigned char *output_data){
  int x, y;
  FILE* fp = fopen(filename, "wb");

  if(fp==NULL){
    printf("fp error! \r\n");
    fclose(fp);
    return;
  }

  fprintf(fp, "P5\n%d %d\n255\n", RES_COLS, RES_ROWS);

  unsigned char *raw = (unsigned char *)malloc( sizeof(unsigned char) *
      RES_COLS * RES_ROWS);
  for(int i = 0 ; i < RES_ROWS*RES_COLS; i++){
    y = i/RES_COLS;
    x = i%RES_COLS;
    raw[i]=output_data[y*RES_COLS + x];
  }

  fwrite(raw, sizeof(unsigned char), (RES_ROWS)*(RES_COLS), fp);
  return;
}


void calcToDisp(unsigned int *disp, unsigned char *calc,
    const unsigned int idx){
  unsigned int *dispPtr = disp;
  unsigned char *calcPtr = calc;

  dispPtr[idx] = (unsigned int)calcPtr[idx];
  dispPtr[idx] = (dispPtr[idx] << 8) + (unsigned int)calcPtr[idx];
  dispPtr[idx] = (dispPtr[idx] << 8) + (unsigned int)calcPtr[idx];
}
