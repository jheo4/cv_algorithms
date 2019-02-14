#include "defines.h"
#include "init.h"
#include "wind.h"
#include "headers.h"
#include "pgmreader.h"

int main(int argc, char **argv)
{
  Options options(argc, argv);
  profile = options.has("profile");

  inputL = (cl_uchar*)alignedMalloc(sizeof(cl_uchar) * (ROWS) * (COLS));
  inputR = (cl_uchar*)alignedMalloc(sizeof(cl_uchar) * (ROWS) * (COLS));
  disp_input = (cl_uint*)alignedMalloc(sizeof(cl_uint) * (ROWS) * (COLS));

  output = (cl_uchar*)alignedMalloc(sizeof(cl_uchar)*(RES_ROWS)*(RES_COLS));
  disp_output = (cl_uint*)alignedMalloc(sizeof(cl_uint)*(RES_ROWS)*(RES_COLS));


  // Image File Name Set
  inputImgL = "imgL.pgm";
  inputImgR = "imgR.pgm";
  outputFileName = "result.pgm";
  // Init...
  initSDL();
  initCL();
  pgmReader(inputImgL.c_str(), (unsigned char*)inputL,
      (unsigned int*)disp_input);
  pgmReader(inputImgR.c_str(), (unsigned char*)inputR,
      (unsigned int*)disp_input);

  //pgmWriter(outputFileName.c_str(), inputL);
  eventLoop();
}
