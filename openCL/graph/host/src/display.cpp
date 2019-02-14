//
//    Author : Jin Heo
//    Date : 2017 09 09
//

#include "defines.h"
#include "init.h"
#include "wind.h"
#include "headers.h"
#include "pgmreader.h"

int main(int argc, char **argv)
{
  Options options(argc, argv);
  profile = options.has("profile");
  input = (cl_uchar*)alignedMalloc(sizeof(cl_uchar) * ROWS * COLS);
  disp_input = (cl_uint*)alignedMalloc(sizeof(cl_uint) * ROWS * COLS);
  output = (cl_uchar*)alignedMalloc(sizeof(cl_uchar) * ROWS * COLS);
  disp_output = (cl_uint*)alignedMalloc(sizeof(cl_uint) * ROWS * COLS);

  // Image File Name Set
  inputFileName = "test.pgm";
  outputFileName = "result.pgm";
  // Init...
  initSDL();
  initCL();
  pgmReader(inputFileName.c_str(), COLS, ROWS, (unsigned char*)input);
  eventLoop();
}
