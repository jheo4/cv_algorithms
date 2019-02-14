//
//  Author : Jin Heo
//  Date : Nov, 9, 2017
//

// PPM R/W functions
void ppmReader(const char *_fileName, unsigned int *_inputData, int _rows,
    int _cols);
void ppmWriter(const char*_fileName, unsigned int *_outputData, int _rows,
    int _cols);

// PGM R/W functions
void pgmReader(const char *filename, unsigned char *calc_data, int _rows,
    int _cols);
void pgmReader(const char *filename, unsigned char *calc_data,
    unsigned int *disp_data, int _rows, int _cols);
void pgmWriter(const char *filename, unsigned char *output_data, int _rows,
    int _cols);

// to display PGM or PPM, it must be converted to RGB.
void calcToDisp(unsigned int *disp, unsigned char *calc,
    const unsigned int idx);
