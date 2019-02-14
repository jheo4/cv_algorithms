//
//  Author : Jin Heo
//  Date : Nov, 9, 2017
//  Description : Functions for executing the SIFT graph
//

void graphExe();

// host functions for calling FPGA kernels.
void imagePyramid4(int rows, int cols, unsigned char *layer0,
    unsigned char *layer1, unsigned char *layer2, unsigned char *layer3);
void scaleImage(unsigned char *_input, int *_inputRows, int *_inputCols,
    unsigned char *_output, int *_outputRows, int *_outputCols);
void gaussianBlur(unsigned char *_input, int _rows, int _cols, float _k,
    unsigned char **_output);
void dog(unsigned char **_input, int _rows, int _cols,
    unsigned char **_output);
void keypointFunc(unsigned char **_dogInput, unsigned char **_blurInput,
    int _rows, int _cols, keypoint *_output);
void keypointDescriptor(unsigned char **_input, keypoint *_keys, int _rows,
    int _cols, keypoint *_outputKey, keypointDesc *_output);

// Draw keypoint for checking the result.
void testDraw(unsigned char *_img, keypoint *_key, int _rows, int _cols,
    char *fileName);
