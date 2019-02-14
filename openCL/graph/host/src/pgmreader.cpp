//
//    Author : Jin Heo
//    Date : 2017 09 09
//

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include "pgmreader.h"
void pgmReader(const char *filename, const unsigned int width,
    const unsigned int height, unsigned char *input_data){
  FILE *fp = fopen(filename, "rb");
  if(fp ==NULL){
    printf("fp error! \r\n");
    fclose(fp);
    return;
  }
  const size_t headerSize = 0x40;
  char header[headerSize];

  fgets(header, headerSize, fp);
  printf("%s", header);
  fgets(header, headerSize, fp);
  printf("%s", header);
  fgets(header, 0x20, fp);
  printf("%s", header);

  unsigned char *raw = (unsigned char *)malloc(sizeof(unsigned char) *
      width * height);
  fread(raw, sizeof(unsigned char), width * height, fp);

  unsigned char *raw_ptr = raw;
  unsigned char *input_data_ptr = input_data;

  for (int i = 0; i != width*height; ++i) {
    *input_data_ptr++ = *raw_ptr++;
  }
  free(raw);
  fclose(fp);
  printf("ok!\r\n");
}

void pgmWriter(const char *filename, const unsigned int width,
    const unsigned int height, unsigned char *output_data){
  int x, y;
  FILE* fp = fopen(filename, "wb");

  if(fp==NULL){
    printf("fp error! \r\n");
    fclose(fp);
    return;
  }

  fprintf(fp, "P5\n%d %d\n255\n", width, height);

  unsigned char *raw = (unsigned char *)malloc( sizeof(unsigned char) *
      width * height);

  for(int i = 0 ; i < width*height; i++){
    raw[i] = output_data[i];
  }

  fwrite(raw, sizeof(unsigned char), width*height, fp);
  return;
}

