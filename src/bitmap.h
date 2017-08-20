#ifndef BITMAP_H
#define BITMAP_H

typedef struct {
  int width, height;
  unsigned char* data;
} Bitmap;

Bitmap* CreateBitmap(const char filename[]) {
  Bitmap* bmp = (Bitmap*) malloc(sizeof(Bitmap));

  unsigned char header[54];
  unsigned int data_pos;
  unsigned int image_size;
  unsigned char* data;

  FILE* file = fopen(filename, "rb");
  if (!file) {
    printf("Problem opening file %s.\n", filename);
    return 0;
  }
  
  if (fread(header, 1, 54, file) != 54) { 
    printf("%s is not a correct BMP file\n", filename);
    fclose(file);
    return 0;
  }

  if (header[0] != 'B' || header[1] != 'M') {
    printf("%s is not a correct BMP file\n", filename);
    fclose(file);
    return 0;
  }

  // Make sure this is a 24bpp file.
  if (*(int*)&(header[0x1E]) != 0 ) { printf("%s is not a correct BMP file\n", filename); fclose(file); return 0; }
  if (*(int*)&(header[0x1C]) != 24) { printf("%s is not a correct BMP file\n", filename); fclose(file); return 0; }

  data_pos    = *(int*)&(header[0x0A]);
  image_size  = *(int*)&(header[0x22]);
  bmp->width  = *(int*)&(header[0x12]);
  bmp->height = *(int*)&(header[0x16]);

  if (image_size == 0) image_size = bmp->width * bmp->height * 3; 
  if (data_pos   == 0) data_pos = 54;

  bmp->data = (unsigned char*) malloc(image_size);
  fread(bmp->data, 1, image_size, file);

  fclose (file);
  return bmp;
}

unsigned char GetComponent(Bitmap* bmp, int x, int y, int color) {
  return bmp->data[(y * bmp->width + x) * 3 + color];
}

#endif
