//
//  Author : Jin Heo
//  Date : Nov, 9, 2017
//

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include "fileReader.h"
#include "defines.h"

void ppmReader(const char *_fileName, unsigned int *_inputData,
    int _rows = ROWS, int _cols = COLS){
  FILE *fp = NULL;
  fp = fopen(_fileName, "rb");
  if(fp ==NULL){
    printf("ppm reader fp error! \r\n");
    fclose(fp);
    return;
  }
  const size_t headerSize = 0x40;
  char header[headerSize];

  fgets(header, headerSize, fp);
  //printf("%s", header);
  fgets(header, headerSize, fp);
  //printf("%s", header);
  fgets(header, 0x20, fp);
  //printf("%s", header);

  unsigned char *raw = (unsigned char *)malloc(sizeof(unsigned char) *
      _rows * _cols *3);
  fread(raw, sizeof(unsigned char), _rows*_cols*3, fp);

  int result;
  unsigned char *rawPtr = raw;
  unsigned int *inputPtr = _inputData;

  for (unsigned int i = 0 ; i < _rows*_cols ; i++) {
    *inputPtr++ = *rawPtr + (*++rawPtr << 8) + (*++rawPtr << 16);
    *rawPtr++;
  }
  free(raw);
  fclose(fp);
  //printf("read ppm!\r\n");
}


void ppmWriter(const char*_fileName, unsigned int *_outputData,
    int _rows = ROWS, int _cols = COLS){
  int x, y;
  FILE* fp = fopen(_fileName, "wb");

  if(fp==NULL){
    printf("fp error! \r\n");
    fclose(fp);
    return;
  }

  fprintf(fp, "P6\n%d %d\n255\n", _cols, _rows);

  unsigned char *raw = (unsigned char *)malloc( sizeof(unsigned char) *
      _rows * _cols * 3);
  unsigned char *rawPtr = raw;
  unsigned int *outputPtr = _outputData;

  for(int i = 0 ; i < _rows*_cols; i++){
    *rawPtr++ = (*outputPtr) & 0xff;
    *rawPtr++ = (*outputPtr >> 8) & 0xff;
    *rawPtr++ = ((*outputPtr++) >> 16) & 0xff;
  }
  fwrite(raw, sizeof(unsigned char), _rows*_cols*3, fp);
  free(raw);
  fclose(fp);

  return;
}


void pgmReader(const char *filename, unsigned char *calc_data,
    int _rows = ROWS, int _cols = COLS){
  FILE *fp = NULL;
  fp = fopen(filename, "rb");
  if(fp ==NULL){
    printf("pgm reader fp error! \r\n");
    fclose(fp);
    return;
  }
  const size_t headerSize = 0x40;
  char header[headerSize];

  // remove the header
  fgets(header, headerSize, fp);
  //printf("%s", header);
  fgets(header, headerSize, fp);
  //printf("%s", header);
  fgets(header, 0x20, fp);
  //printf("%s", header);

  //unsigned char *raw = (unsigned char *)malloc(sizeof(unsigned char)
  //    * _rows * _cols);
  fread(calc_data, sizeof(unsigned char), _rows*_cols, fp);

  // transferring data from FILE to calcArr and dispArr...
  //for (int i = 0 ; i != _rows*_cols ; i++) {
  //  calc_data[i] = raw[i];
  //}
  //free(raw);
  fclose(fp);
}

void pgmWriter(const char *filename, unsigned char *output_data,
    int _rows = ROWS, int _cols = COLS){
  int x, y;
  FILE* fp = fopen(filename, "wb");
  if(fp==NULL){
    printf("fp error! \r\n");
    fclose(fp);
    return;
  }

  fprintf(fp, "P5\n%d %d\n255\n", _cols, _rows);
  fwrite(output_data, sizeof(unsigned char), (_rows)*(_cols), fp);
  fclose(fp);

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

void pgmReader(const char *filename, unsigned char *calc_data,
    unsigned int *disp_data, int _rows = ROWS, int _cols = COLS){
  FILE *fp = NULL;
  fp = fopen(filename, "rb");
  if(fp ==NULL){
    printf("pgm reader fp error! \r\n");
    fclose(fp);
    return;
  }
  const size_t headerSize = 0x40;
  char header[headerSize];

  // remove the header
  fgets(header, headerSize, fp);
  //printf("%s", header);
  fgets(header, headerSize, fp);
  //printf("%s", header);
  fgets(header, 0x20, fp);
  //printf("%s", header);

  unsigned char *raw = (unsigned char *)malloc(sizeof(unsigned char) *
      _rows * _cols);
  fread(raw, sizeof(unsigned char), _rows*_cols, fp);

  // transferring data from FILE to calcArr and dispArr...
  for (int i = 0 ; i != _rows*_cols ; i++) {
    calc_data[i] = raw[i];
    calcToDisp(disp_data, calc_data, i);
  }
  free(raw);
  fclose(fp);
  //printf("read pgm!\r\n");
}
