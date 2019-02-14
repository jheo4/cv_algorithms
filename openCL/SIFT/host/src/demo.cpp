//
//  Author : Jin Heo
//  Date : Nov, 9, 2017
//

#include <math.h>
#include "headers.h"
#include "fileReader.h"
#include "demo.h"

cl_event event;
cl_int status;

// Graph Execution fuction
void graphExe()
{
  int rows = ROWS;
  int cols = COLS;

  // Scaled Images buffer
  unsigned char *img0 = (unsigned char*)alignedMalloc(sizeof(unsigned char) *
      rows * cols);
  unsigned char *img1 = (unsigned char*)alignedMalloc(sizeof(unsigned char) *
      rows/2 * cols/2);
  unsigned char *img2 = (unsigned char*)alignedMalloc(sizeof(unsigned char) *
      rows/4 * cols/4);
  unsigned char *img3 = (unsigned char*)alignedMalloc(sizeof(unsigned char) *
      rows/8 * cols/8);

  // blur Image buffer, 5 layer of each octave
  unsigned char *blurImg0[5];
  unsigned char *blurImg1[5];
  unsigned char *blurImg2[5];
  unsigned char *blurImg3[5];

  // dog Image buffer, 4 layer of each octave
  unsigned char *dogImg0[4];
  unsigned char *dogImg1[4];
  unsigned char *dogImg2[4];
  unsigned char *dogImg3[4];

  // initial keypoints buffer of each octave
  keypoint *keys0 = (keypoint*)alignedMalloc(sizeof(keypoint) * 2);
  keypoint *keys1 = (keypoint*)alignedMalloc(sizeof(keypoint) * 2);
  keypoint *keys2 = (keypoint*)alignedMalloc(sizeof(keypoint) * 2);
  keypoint *keys3 = (keypoint*)alignedMalloc(sizeof(keypoint) * 2);

  // final keypoints buffer (init keys + additional keys)
  keypoint *finKeys0 = (keypoint*)alignedMalloc(sizeof(keypoint) * 2);
  keypoint *finKeys1 = (keypoint*)alignedMalloc(sizeof(keypoint) * 2);
  keypoint *finKeys2 = (keypoint*)alignedMalloc(sizeof(keypoint) * 2);
  keypoint *finKeys3 = (keypoint*)alignedMalloc(sizeof(keypoint) * 2);

  // descriptor buffer
  keypointDesc *desc0 = (keypointDesc*)alignedMalloc(sizeof(keypointDesc) * 2);
  keypointDesc *desc1 = (keypointDesc*)alignedMalloc(sizeof(keypointDesc) * 2);
  keypointDesc *desc2 = (keypointDesc*)alignedMalloc(sizeof(keypointDesc) * 2);
  keypointDesc *desc3 = (keypointDesc*)alignedMalloc(sizeof(keypointDesc) * 2);

  // Produce scaled images...
  imagePyramid4(rows, cols, img0, img1, img2, img3);

	printf("Layer0...\r\n");
  // Layer 0 :
  for(int i = 0 ; i < 5 ; i++)
    blurImg0[i] = (unsigned char*)alignedMalloc(sizeof(unsigned char) *
                  rows * cols);
  for(int i = 0 ; i < 4 ; i++)
    dogImg0[i] = (unsigned char*)alignedMalloc(sizeof(unsigned char) *
                  rows * cols);

  gaussianBlur(img0, rows, cols, 1, blurImg0);
  dog(blurImg0, rows, cols, dogImg0);
  keypointFunc(dogImg0, blurImg0, rows, cols, keys0);
  keypointDescriptor(blurImg0, keys0, rows, cols, finKeys0, desc0);

  testDraw(blurImg0[1], &keys0[0], rows, cols, "test00.pgm");
  testDraw(blurImg0[1], &keys0[1], rows, cols, "test01.pgm");

  for(int i = 0 ; i < 5 ; i++)
    alignedFree(blurImg0[i]);
  for(int i = 0 ; i < 4 ; i++)
    alignedFree(dogImg0[i]);

  printf("num Of Keys 0 %d, 1 %d\r\n", keys0[0].numOfKeys, keys0[1].numOfKeys);
  printf("Fin Keys 0 %d, 1 %d\r\n", finKeys0[0].numOfKeys,
      finKeys0[1].numOfKeys);

  // Layer 1 :
  	printf("Layer1...\r\n");
  rows /= 2;
  cols /= 2;
  for(int i = 0 ; i < 5 ; i++)
    blurImg1[i] = (unsigned char*)alignedMalloc(sizeof(unsigned char) *
        rows * cols);
  for(int i = 0 ; i < 4 ; i++)
    dogImg1[i] = (unsigned char*)alignedMalloc(sizeof(unsigned char) *
        rows * cols);

  gaussianBlur(img1, rows, cols, sqrt(2), blurImg1);
  dog(blurImg1, rows, cols, dogImg1);
  keypointFunc(dogImg1, blurImg1, rows, cols, keys1);
  keypointDescriptor(blurImg1, keys1, rows, cols, finKeys1, desc1);
  testDraw(blurImg1[1], &keys1[0], rows, cols, "test10.pgm");
  testDraw(blurImg1[1], &keys1[1], rows, cols, "test11.pgm");

  for(int i = 0 ; i < 5 ; i++)
    alignedFree(blurImg1[i]);
  for(int i = 0 ; i < 4 ; i++)
    alignedFree(dogImg1[i]);

  printf("num Of Keys 0 %d, 1 %d\r\n", keys1[0].numOfKeys, keys1[1].numOfKeys);
  printf("Fin Keys 0 %d, 1 %d\r\n", finKeys1[0].numOfKeys,
      finKeys1[1].numOfKeys);

  // Layer 2 :
  	printf("Layer2...\r\n");
  rows /= 2;
  cols /= 2;
  for(int i = 0 ; i < 5 ; i++)
    blurImg2[i] = (unsigned char*)alignedMalloc(sizeof(unsigned char) *
        rows * cols);
  for(int i = 0 ; i < 4 ; i++)
    dogImg2[i] = (unsigned char*)alignedMalloc(sizeof(unsigned char) *
        rows * cols);

  gaussianBlur(img2, rows, cols, 2, blurImg2);
  dog(blurImg2, rows, cols, dogImg2);
  keypointFunc(dogImg2, blurImg2, rows, cols, keys2);
  keypointDescriptor(blurImg2, keys2, rows, cols, finKeys2, desc2);
  testDraw(blurImg2[1], &keys2[0], rows, cols, "test20.pgm");
  testDraw(blurImg2[1], &keys2[1], rows, cols, "test21.pgm");

  for(int i = 0 ; i < 5 ; i++)
    alignedFree(blurImg2[i]);
  for(int i = 0 ; i < 4 ; i++)
    alignedFree(dogImg2[i]);
  printf("num Of Keys 0 %d, 1 %d\r\n", keys2[0].numOfKeys, keys2[1].numOfKeys);
  printf("Fin Keys 0 %d, 1 %d\r\n", finKeys2[0].numOfKeys,
      finKeys2[1].numOfKeys);

  // Layer 3 :
  printf("Layer3...\r\n");
  rows /= 2;
  cols /= 2;

  for(int i = 0 ; i < 5 ; i++)
    blurImg3[i] = (unsigned char*)alignedMalloc(sizeof(unsigned char) *
        rows * cols);
  for(int i = 0 ; i < 4 ; i++)
    dogImg3[i] = (unsigned char*)alignedMalloc(sizeof(unsigned char) *
        rows * cols);

  gaussianBlur(img3, rows, cols, 2*sqrt(2), blurImg3);
  dog(blurImg3, rows, cols, dogImg3);
  keypointFunc(dogImg3, blurImg3, rows, cols, keys3);
  keypointDescriptor(blurImg3, keys3, rows, cols, finKeys3, desc3);
  testDraw(blurImg3[1], &keys3[0], rows, cols, "test30.pgm");
  testDraw(blurImg3[1], &keys3[1], rows, cols, "test31.pgm");

  for(int i = 0 ; i < 5 ; i++) alignedFree(blurImg3[i]);
  for(int i = 0 ; i < 4 ; i++) alignedFree(dogImg3[i]);

  printf("num Of Keys 0 %d, 1 %d\r\n", keys3[0].numOfKeys, keys3[1].numOfKeys);
  printf("Fin Keys 0 %d, 1 %d\r\n", finKeys3[0].numOfKeys,
      finKeys3[1].numOfKeys);

  alignedFree(img0);
  alignedFree(img1);
  alignedFree(img2);
  alignedFree(img3);

  alignedFree(keys0);
  alignedFree(keys1);
  alignedFree(keys2);
  alignedFree(keys3);

  alignedFree(finKeys0);
  alignedFree(finKeys1);
  alignedFree(finKeys2);
  alignedFree(finKeys3);

  alignedFree(desc0);
  alignedFree(desc1);
  alignedFree(desc2);
  alignedFree(desc3);

  // Window Title bar
  char title[256];
  sprintf(title, "SIFT Working...");
  SDL_SetWindowTitle(sdlWindow, title);
}

// Generate image pyramid of 4
void imagePyramid4(int rows, int cols, unsigned char *layer0,
    unsigned char *layer1, unsigned char *layer2, unsigned char *layer3){
  if( rows%8 != 0 || cols%8 != 0 ){
    printf("the original image must be mulpile of 8\r\n");
    return;
  }
  int rows0 = rows, cols0 = cols;
  int rows1, cols1;
  int rows2, cols2;
  int rows3, cols3;

  pgmReader(inputImg.c_str(), (unsigned char*)layer0, rows0, cols0);
  scaleImage(layer0, &rows0, &cols0, layer1, &rows1, &cols1);
  scaleImage(layer1, &rows1, &cols1, layer2, &rows2, &cols2);
  scaleImage(layer2, &rows2, &cols2, layer3, &rows3, &cols3);
}

void scaleImage(unsigned char *_input, int *_inputRows, int *_inputCols,
    unsigned char *_output, int *_outputRows, int *_outputCols){
  int inputRows = *_inputRows;
  int inputCols = *_inputCols;
  int outputRows = inputRows / 2;
  int outputCols = inputCols / 2;
  *_outputRows = outputRows;
  *_outputCols = outputCols;

  for(int y = 0 ; y < outputRows; y++){
    for(int x = 0 ; x < outputCols ; x++){
      _output[y*outputCols + x] = _input[(y*2)*(inputCols)+(x*2)];
    }
  }
}

// call gaussian blur FPGA kernel
void gaussianBlur(unsigned char *_input, int _rows, int _cols,
    float _k, unsigned char **_output){
  cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
      sizeof(cl_uchar) * _rows * _cols , NULL, &status);
  cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY,
      sizeof(cl_uchar) * _rows * _cols , NULL, &status);

  clEnqueueWriteBuffer(queue, inputBuffer, CL_FALSE, 0,
      sizeof(cl_uchar) * _rows * _cols, _input, 0, NULL, NULL);
  clFinish(queue);

  // set sigma for Gaussian Blur
  float sigma;
  for(int i = 0 ; i < 5 ; i++){
    // for reducing the error from floating point
    if(i==0) sigma = sqrt(2)*_k;
    if(i==1) sigma = 2*_k;
    if(i==2) sigma = 2*sqrt(2)*_k;
    if(i==3) sigma = 4*_k;
    if(i==4) sigma = 4*sqrt(2)*_k;
    clSetKernelArg(gaussianBlurKernel, 0, sizeof(cl_mem), &inputBuffer);
    clSetKernelArg(gaussianBlurKernel, 1, sizeof(cl_mem), &outputBuffer);
    clSetKernelArg(gaussianBlurKernel, 2, sizeof(float), &sigma);
    clSetKernelArg(gaussianBlurKernel, 3, sizeof(int), &_rows);
    clSetKernelArg(gaussianBlurKernel, 4, sizeof(int), &_cols);

    clEnqueueTask(queue, gaussianBlurKernel, 0, NULL, &event);
    clFinish(queue);

    clEnqueueReadBuffer(queue, outputBuffer, CL_FALSE, 0,
        sizeof(cl_uchar) * _rows * _cols, _output[i], 0, NULL, NULL);
    clFinish(queue);
  }

  clReleaseMemObject(inputBuffer);
  clReleaseMemObject(outputBuffer);
}

// call DoG FPGA kernel
void dog(unsigned char **_input, int _rows, int _cols, unsigned char **_output){
  cl_mem input1Buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
      sizeof(cl_uchar) * _rows * _cols , NULL, &status);
  cl_mem input2Buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
      sizeof(cl_uchar) * _rows * _cols , NULL, &status);
  cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY,
      sizeof(cl_uchar) * _rows * _cols , NULL, &status);

  for(int i = 0 ; i < 4; i++){
    clEnqueueWriteBuffer(queue, input1Buffer, CL_FALSE, 0,
        sizeof(cl_uchar) * _rows * _cols, _input[i], 0, NULL, NULL);
    clFinish(queue);
    clEnqueueWriteBuffer(queue, input2Buffer, CL_FALSE, 0,
        sizeof(cl_uchar) * _rows * _cols, _input[i+1], 0, NULL, NULL);
    clFinish(queue);

    clSetKernelArg(dogKernel, 0, sizeof(cl_mem), &input1Buffer);
    clSetKernelArg(dogKernel, 1, sizeof(cl_mem), &input2Buffer);
    clSetKernelArg(dogKernel, 2, sizeof(cl_mem), &outputBuffer);
    clSetKernelArg(dogKernel, 3, sizeof(int), &_rows);
    clSetKernelArg(dogKernel, 4, sizeof(int), &_cols);

    clEnqueueTask(queue, dogKernel, 0, NULL, &event);
    clFinish(queue);

    clEnqueueReadBuffer(queue, outputBuffer, CL_FALSE, 0,
        sizeof(cl_uchar) * _rows * _cols, _output[i], 0, NULL, NULL);
    clFinish(queue);
  }
  clReleaseMemObject(input1Buffer);
  clReleaseMemObject(input2Buffer);
  clReleaseMemObject(outputBuffer);
}

// call keypoint FPGA kernel
void keypointFunc(unsigned char **_dogInput, unsigned char **_blurInput,
    int _rows, int _cols, keypoint *_output){
  cl_mem dogBuffer[3];
  cl_mem blurBuffer[3];
  for(int i =  0 ; i < 3 ; i++){
    dogBuffer[i] = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
        sizeof(cl_uchar) * _rows * _cols , NULL, &status);
    blurBuffer[i] = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
        sizeof(cl_uchar) * _rows * _cols , NULL, &status);
  }
  cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY,
        sizeof(keypoint) , NULL, &status);

  for(int i = 0 ; i < 2; i++){
    if(i == 0){
      clEnqueueWriteBuffer(queue, dogBuffer[0], CL_FALSE, 0,
          sizeof(cl_uchar) * _rows * _cols, _dogInput[0], 0, NULL, NULL);
      clFinish(queue);
      clEnqueueWriteBuffer(queue, dogBuffer[1], CL_FALSE, 0,
          sizeof(cl_uchar) * _rows * _cols, _dogInput[1], 0, NULL, NULL);
      clFinish(queue);
      clEnqueueWriteBuffer(queue, dogBuffer[2], CL_FALSE, 0,
          sizeof(cl_uchar) * _rows * _cols, _dogInput[2], 0, NULL, NULL);
      clFinish(queue);
      clEnqueueWriteBuffer(queue, blurBuffer[0], CL_FALSE, 0,
          sizeof(cl_uchar) * _rows * _cols, _blurInput[0], 0, NULL, NULL);
      clFinish(queue);
      clEnqueueWriteBuffer(queue, blurBuffer[1], CL_FALSE, 0,
          sizeof(cl_uchar) * _rows * _cols, _blurInput[1], 0, NULL, NULL);
      clFinish(queue);
      clEnqueueWriteBuffer(queue, blurBuffer[2], CL_FALSE, 0,
          sizeof(cl_uchar) * _rows * _cols, _blurInput[2], 0, NULL, NULL);
      clFinish(queue);
    }
    else{
      clEnqueueWriteBuffer(queue, dogBuffer[0], CL_FALSE, 0,
          sizeof(cl_uchar) * _rows * _cols, _dogInput[1], 0, NULL, NULL);
      clFinish(queue);
      clEnqueueWriteBuffer(queue, dogBuffer[1], CL_FALSE, 0,
          sizeof(cl_uchar) * _rows * _cols, _dogInput[2], 0, NULL, NULL);
      clFinish(queue);
      clEnqueueWriteBuffer(queue, dogBuffer[2], CL_FALSE, 0,
          sizeof(cl_uchar) * _rows * _cols, _dogInput[3], 0, NULL, NULL);
      clFinish(queue);
      clEnqueueWriteBuffer(queue, blurBuffer[0], CL_FALSE, 0,
          sizeof(cl_uchar) * _rows * _cols, _blurInput[2], 0, NULL, NULL);
      clFinish(queue);
      clEnqueueWriteBuffer(queue, blurBuffer[1], CL_FALSE, 0,
          sizeof(cl_uchar) * _rows * _cols, _blurInput[3], 0, NULL, NULL);
      clFinish(queue);
      clEnqueueWriteBuffer(queue, blurBuffer[2], CL_FALSE, 0,
          sizeof(cl_uchar) * _rows * _cols, _blurInput[4], 0, NULL, NULL);
      clFinish(queue);
    }
    for(int j = 0 ; j < 3 ; j++){
      clSetKernelArg(keyKernel, j, sizeof(cl_mem), &dogBuffer[j]);
      clSetKernelArg(keyKernel, j+3, sizeof(cl_mem), &blurBuffer[j]);
    }
    clSetKernelArg(keyKernel, 6, sizeof(cl_mem), &outputBuffer);

    clSetKernelArg(keyKernel, 7, sizeof(int), &_rows);
    clSetKernelArg(keyKernel, 8, sizeof(int), &_cols);

    clEnqueueTask(queue, keyKernel, 0, NULL, &event);
    clFinish(queue);
    clEnqueueReadBuffer(queue, outputBuffer, CL_FALSE, 0, sizeof(keypoint),
        (void *)&_output[i], 0, NULL, NULL);
    clFinish(queue);
  }

  for(int i = 0 ; i < 3 ; i++){
    clReleaseMemObject(dogBuffer[i]);
    clReleaseMemObject(blurBuffer[i]);
  }
  clReleaseMemObject(outputBuffer);
}

// call keyDescriptor FPGA kernel
void keypointDescriptor(unsigned char **_input, keypoint *_keys, int _rows,
    int _cols, keypoint *_outputKey, keypointDesc *_output){
  cl_mem imgBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
      sizeof(cl_uchar) * _rows * _cols , NULL, &status);
  cl_mem keyBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
      sizeof(keypoint) , NULL, &status);
  cl_mem keyOutput = clCreateBuffer(context, CL_MEM_READ_ONLY,
      sizeof(keypoint) , NULL, &status);
  cl_mem descOutput = clCreateBuffer(context, CL_MEM_READ_ONLY,
      sizeof(keypointDesc) , NULL, &status);
  float sigma;
  for(int i = 0 ; i < 2; i++){
    sigma = 2 + (2*i);

    if(i == 0) clEnqueueWriteBuffer(queue, imgBuffer, CL_FALSE, 0,
        sizeof(cl_uchar) * _rows * _cols, _input[1], 0, NULL, NULL);
    else clEnqueueWriteBuffer(queue, imgBuffer, CL_FALSE, 0,
        sizeof(cl_uchar) * _rows * _cols, _input[3], 0, NULL, NULL);
    clFinish(queue);
    clEnqueueWriteBuffer(queue, keyBuffer, CL_FALSE, 0, sizeof(keypoint),
        (const void*)&_keys[i], 0, NULL, NULL);
    clFinish(queue);

    clSetKernelArg(descKernel, 0, sizeof(cl_mem), &imgBuffer);
    clSetKernelArg(descKernel, 1, sizeof(float), &sigma);
    clSetKernelArg(descKernel, 2, sizeof(cl_mem), &keyBuffer);
    clSetKernelArg(descKernel, 3, sizeof(cl_mem), &keyOutput);
    clSetKernelArg(descKernel, 4, sizeof(cl_mem), &descOutput);
    clSetKernelArg(descKernel, 5, sizeof(int), &_rows);
    clSetKernelArg(descKernel, 6, sizeof(int), &_cols);

    clEnqueueTask(queue, descKernel, 0, NULL, &event);
    clFinish(queue);

    clEnqueueReadBuffer(queue, keyOutput, CL_FALSE, 0, sizeof(keypoint),
        (void *)&_outputKey[i], 0, NULL, NULL);
    clFinish(queue);
    clEnqueueReadBuffer(queue, descOutput, CL_FALSE, 0, sizeof(keypointDesc),
        (void *)&_output[i], 0, NULL, NULL);
    clFinish(queue);
  }
  clReleaseMemObject(imgBuffer);
  clReleaseMemObject(keyBuffer);
  clReleaseMemObject(keyOutput);
  clReleaseMemObject(descOutput);
}


// Test Draw...
// make a rectangle near each keypoint
void testDraw(unsigned char *_img, keypoint *_key, int _rows, int _cols,
    char *fileName){
	unsigned char temp[_rows*_cols];
	for(int i = 0 ; i < _rows*_cols ; i++){
		temp[i] = _img[i];
	}
	for(int i = _key->numOfKeys - 1; i >= 0 ; i--){
		int y = _key->y[i];
		int x = _key->x[i];
		temp[y*_cols + x] = 255;

		for(int j = - 2 ; j <= 2; j++){
			for(int k = -2 ; k <= 2 ; k++){
				if(j == -2 || j == 2 || k == -2 || k ==2){
					temp[(y+j)*_cols + (x+k)] = 255;
				}
			}
		}
	}
	pgmWriter(fileName, temp, _rows, _cols);
}
