//
//  Author : Jin Heo
//  Date : Nov, 9, 2017
//  Description : definitions and structure of keypoint and key descriptor
//

// Init rows & cols
#define ROWS 800
#define COLS 800

#define PI 3.14159265

// Max num of key points
#define MAXKEYPONT 1500

// Keypoint structure
#ifndef KEYSTRUCT
#define KEYSTRUCT
typedef struct _keypoint{
  unsigned int x[MAXKEYPONT];
  unsigned int y[MAXKEYPONT];
  unsigned int orientation[MAXKEYPONT];
  unsigned int numOfKeys;
}keypoint;
#endif

// Key descriptor structure
// magnitude is normalized within 0~255 value.
#ifndef KEYDESC
#define KEYDESC
typedef struct _keypointDesc{
  unsigned char dir[MAXKEYPONT][16];
  unsigned char mag[MAXKEYPONT][16];
}keypointDesc;
#endif
