> Author : Jin Heo

> E-mail : 993jin@gmail.com

OpenCL SIFT
=============
This is an application of SIFT for Intel Vision Algorithm Designer.

FPGA Application
-------------
1. image scaling kernel

* void cl_imgDownSampling(uchar* inputImg, uchar* outputImg, int rows, int cols)
  - inputImg : an original image for down sizing
  - outputImg : an downsized image
  - rows : hegith of the image
  - cols : width of the image

2. gaussian blur kernel

* void cl_gaussianBlur(uchar* inputImg, uchar* outputImg, float sigma, int rows, int cols)
  - inputImg : an original image for blurring
  - outputImg : an blurred image
  - sigma : a sigma value for the gaussian expression
  - rows : hegith of the image
  - cols : width of the image

3. dog kernel

* void cl_dog(uchar* lessBlurred, uchar* moreBlurred, uchar* ouput, int rows, int cols)
  - lessBlurred : a less blurred image
  - moreBlurred : a more blurred image
  - output : the DoG image (lessBlurred - moreBlurred)
  - rows : hegith of the image
  - cols : width of the image
  
4. keypoint kerenl

* void cl_keyPoint(uchar* upperLayer, uchar* midLayer, uchar* lowerLayer, uchar* upperImg, uchar* midImg, uchar* lowerImg, keypoint* keypoints, int rows, int cols)
  - upperLayer : DoG n-1 layer
  - midLayer : DoG n layer
  - lowerLayer : DoG n layer
  - upperImg : Gaussian blurred n-1 image
  - midImg : Gaussian blurred n image
  - lowerImg : Gaussian blurred n+1 image
  - keypoints : output keypoints
    - keypoint structure : struct keypoint{ uint x[MAXKEY], uint y[MAXKEY], int orientation[MAXKEY], int numOfKeys }
  - rows : hegith of the image
  - cols : width of the image

5. keypoint descriptor kernel
* void keyDescriptor(uchar* inputImg, float sigma, keypoint* inputKey, keypoint* outputKey, keypointDesc* outputDesc, int rows, int cols)
  - inputImg : a gaussian image for getting descriptors
  - sigma : a sigma value for the gaussian blur to magnitudes
  - inputKey : input keys for getting additional keys and descriptor
  - outputKey :  input keys + additional keys(the magnitude valus (0.8, 1] is the condition for additional keys )
  - outputDesc :  output descriptor for each key
  - rows : hegith of the image
  - cols : width of the image

Host Application
-------------
* inc : header files of the host application
* src : body files of the host application
  - defines.h : definitions of the host application, including the keypoint structure and the key descriptor structure
  - demo : fuctions for calling FPGA kernel functions
  - fileReader : file read / writer functions of PGM, PPM files
  - headers : global variables including CL kerenls and CL configuration variables
  - init : initializing functions for the application
  - wind : Window functions and the event loop

How to use this application
-------------
About the keyboard press events, refer [keyboardPressEvent fuction]. The results are saved as files.

The result of OpenCL SIFT for FPGA
-------------
![OpenCL SIFT Example](https://github.com/sajjadt/vision_fpga_acceleration/blob/master/openCL/basicKernel/SIFT/example2.jpg "The result of OpenCL SIFT for FPGA")

[keyboardPressEvent fuction]: https://github.com/sajjadt/vision_fpga_acceleration/blob/master/openCL/basicKernel/SIFT/host/src/wind.cpp

