//
//
//  Author : Jin Heo
//  Date : Oct, 3, 2017
//

#include "defines.h"
#include "init.h"
#include "wind.h"
#include "headers.h"
#include "fileReader.h"

int main(int argc, char **argv)
{
  Options options(argc, argv);
  profile = options.has("profile");

  input = (cl_uint*)alignedMalloc(sizeof(cl_uint) * (ROWS) * (COLS));

  outputGray = (cl_uchar*)alignedMalloc(sizeof(cl_uchar)*(ROWS)*(COLS));
  output = (cl_uint*)alignedMalloc(sizeof(cl_uint)*(ROWS)*(COLS));


  // Image File Name Set
  inputImg = "img.ppm";
  outputFileName = "rgbResult.ppm";
  outputGrayFileName = "grayResult.pgm";

  // Load the source img
  ppmReader(inputImg.c_str(), input);

  // Init...
  initSDL();
  initCL();
  eventLoop();
}
