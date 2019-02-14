//
//  Author : Jin Heo
//  Date : Nov, 9, 2017
//

#include "defines.h"
#include "init.h"
#include "wind.h"
#include "headers.h"
#include "fileReader.h"

// main function
int main(int argc, char **argv)
{
  Options options(argc, argv);
  profile = options.has("profile");

  // input image buffer
  input = (cl_uchar*)alignedMalloc(sizeof(cl_uchar) * (ROWS) * (COLS));

  // Image File Name Set
  inputImg = "img.pgm";


  // Init...
  initSDL();
  initCL();

  eventLoop();
}
