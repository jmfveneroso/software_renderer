#ifndef BITMAP_H
#define BITMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>

typedef struct {
  int width, height;
  unsigned char* data;
} Bitmap;

Bitmap* CreateBitmap(const char filename[]);

unsigned char GetComponent(Bitmap* bmp, int x, int y, int color);

GLuint loadBMP_custom(const char* imagepath);

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint loadDDS(const char * imagepath);

#endif
