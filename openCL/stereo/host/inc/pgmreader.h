void pgmReader(const char *filename, unsigned char *input,
    unsigned int *disp_input);
void pgmWriter(const char *filename, unsigned char *output_data);
void calcToDisp(unsigned int *disp, unsigned char *calc,
    const unsigned int idx);
