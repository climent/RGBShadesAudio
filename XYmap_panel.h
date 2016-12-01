// Helper functions for a two-dimensional XY matrix of pixels.
// Special credit to Mark Kriegsman
//
// 2014-10-18 - Special version for RGB Shades Kickstarter
//              https://www.kickstarter.com/projects/macetech/rgb-led-shades
//              2014-10-18 - code version 2c (local table, holes are r/w),
//              by Mark Kriegsman
//
//              This special 'XY' code lets you program the RGB Shades
//              as a plain 16x5 matrix.
//
//              Writing to and reading from the 'holes' in the layout is
//              also allowed; holes retain their data, it's just not displayed.
//
//              You can also test to see if you're on or off the layout
//              like this
//                if( XY(x,y) > LAST_VISIBLE_LED ) { ...off the layout...}
//
//              X and Y bounds checking is also included, so it is safe
//              to just do this without checking x or y in your code:
//                leds[ XY(x,y) ] == CRGB::Red;
//              All out of bounds coordinates map to the first hidden pixel.
//
//     XY(x,y) takes x and y coordinates and returns an LED index number,
//             for use like this:  leds[ XY(x,y) ] == CRGB::Red;


// Params for width and height
const uint8_t kMatrixWidth =  15;
const uint8_t kMatrixHeight = 15;

// Pixel layout
//
//      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
//   +------------------------------------------------
// 0 |  .  0  1  2  3  4  5  6  7  8  9 10 11 12 13  .
// 1 | 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14
// 2 | 30 31 32 33 34 35 36  .  . 37 38 39 40 41 42 43
// 3 | 57 56 55 54 53 52 51  .  . 50 49 48 47 46 45 44
// 4 |  . 58 59 60 61 62  .  .  .  . 63 64 65 66 67  .

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds[ NUM_LEDS ];


// This function will return the right 'led index number' for
// a given set of X and Y coordinates on your RGB Shades.
// This code, plus the supporting 80-byte table is much smaller
// and much faster than trying to calculate the pixel ID with code.
#define LAST_VISIBLE_LED 217
uint8_t XY( uint8_t x, uint8_t y)
{
  // any out of bounds address maps to the first hidden pixel
  if ( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
    return (LAST_VISIBLE_LED + 1);
  }

  const uint8_t ShadesTable[] = {
    14, 218, 43, 219, 72, 220, 101, 221, 130, 222, 159, 223, 188, 224, 217,
    13,  15, 42,  44, 71,  73, 100, 102, 129, 131, 158, 160, 187, 189, 216,
    12,  16, 41,  45, 70,  74,  99, 103, 128, 132, 157, 161, 186, 190, 215,
    11,  17, 40,  46, 69,  75,  98, 104, 127, 133, 156, 162, 185, 191, 214,
    10,  18, 39,  47, 68,  76,  97, 105, 126, 134, 155, 163, 184, 192, 213,
    9,  19, 38,  48, 67,  77,  96, 106, 125, 135, 154, 164, 183, 193, 212,
    8,  20, 37,  49, 66,  78,  95, 107, 124, 136, 153, 165, 182, 194, 211,
    7,  21, 36,  50, 65,  79,  94, 108, 123, 137, 152, 166, 181, 195, 210,
    6,  22, 35,  51, 64,  80,  93, 109, 122, 138, 151, 167, 180, 196, 209,
    5,  23, 34,  52, 63,  81,  92, 110, 121, 139, 150, 168, 179, 197, 208,
    4,  24, 33,  53, 62,  82,  91, 111, 120, 140, 149, 169, 178, 198, 207,
    3,  25, 32,  54, 61,  83,  90, 112, 119, 141, 148, 170, 177, 199, 206,
    2,  26, 31,  55, 60,  84,  89, 113, 118, 142, 147, 171, 176, 200, 205,
    1,  27, 30,  56, 59,  85,  88, 114, 117, 143, 146, 172, 175, 201, 204,
    0,  28, 29,  57, 58,  86,  87, 115, 116, 144, 145, 173, 174, 202, 203,
  };

  uint8_t i = (y * kMatrixWidth) + x;
  uint8_t j = ShadesTable[i];
  return j;
}

const uint8_t yCoords[NUM_LEDS] = {
  0,  17,  34,  51,  68,  85, 102, 119, 136, 153, 170, 187, 204, 221, 238,
  9,  26,  43,  60,  77,  94, 111, 128, 145, 162, 179, 196, 213, 230,
  0,  17,  34,  51,  68,  85, 102, 119, 136, 153, 170, 187, 204, 221, 238,
  9,  26,  43,  60,  77,  94, 111, 128, 145, 162, 179, 196, 213, 230,
  0,  17,  34,  51,  68,  85, 102, 119, 136, 153, 170, 187, 204, 221, 238,
  9,  26,  43,  60,  77,  94, 111, 128, 145, 162, 179, 196, 213, 230,
  0,  17,  34,  51,  68,  85, 102, 119, 136, 153, 170, 187, 204, 221, 238,
  9,  26,  43,  60,  77,  94, 111, 128, 145, 162, 179, 196, 213, 230,
  0,  17,  34,  51,  68,  85, 102, 119, 136, 153, 170, 187, 204, 221, 238,
  9,  26,  43,  60,  77,  94, 111, 128, 145, 162, 179, 196, 213, 230,
  0,  17,  34,  51,  68,  85, 102, 119, 136, 153, 170, 187, 204, 221, 238,
  9,  26,  43,  60,  77,  94, 111, 128, 145, 162, 179, 196, 213, 230,
  0,  17,  34,  51,  68,  85, 102, 119, 136, 153, 170, 187, 204, 221, 238,
  9,  26,  43,  60,  77,  94, 111, 128, 145, 162, 179, 196, 213, 230,
  0,  17,  34,  51,  68,  85, 102, 119, 136, 153, 170, 187, 204, 221, 238,
};
const uint8_t xCoords[NUM_LEDS] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   17,  17,  17,  17,  17,  17,  17,  17,  17,  17,  17,  17,  17,  17,
   34,  34,  34,  34,  34,  34,  34,  34,  34,  34,  34,  34,  34,  34,  34,
   51,  51,  51,  51,  51,  51,  51,  51,  51,  51,  51,  51,  51,  51,
   68,  68,  68,  68,  68,  68,  68,  68,  68,  68,  68,  68,  68,  68,  68,
   85,  85,  85,  85,  85,  85,  85,  85,  85,  85,  85,  85,  85,  85,
  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102,
  119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119,
  136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136,
  153, 153, 153, 153, 153, 153, 153, 153, 153, 153, 153, 153, 153, 153,
  170, 170, 170, 170, 170, 170, 170, 170, 170, 170, 170, 170, 170, 170, 170,
  187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187,
  204, 204, 204, 204, 204, 204, 204, 204, 204, 204, 204, 204, 204, 204, 204,
  221, 221, 221, 221, 221, 221, 221, 221, 221, 221, 221, 221, 221, 221,
  238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
};

