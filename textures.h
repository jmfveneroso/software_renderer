#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdio.h>
#include <png.h>

int x, y;

int width, height;
png_byte color_type;
png_byte bit_depth;

png_structp png_ptr;
png_infop info_ptr;
int number_of_passes;
png_bytep * row_pointers;

typedef struct {
  int width, height;
  png_byte color_type;
  png_byte bit_depth;
  
  png_structp png_ptr;
  png_infop info_ptr;
  int number_of_passes;
  png_bytep * row_pointers;

  unsigned char *rgb;
} Texture;

void read_png_file(const char file_name[], Texture* texture) {
  char header[8];    // 8 is the maximum size that can be checked

  /* open file and test for it being a png */
  FILE *fp = fopen(file_name, "rb");
  if (!fp)
    printf("[read_png_file] File %s could not be opened for reading", file_name);
  fread(header, 1, 8, fp);
  // if (png_sig_cmp(header, 0, 8))
  //   abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);


  /* initialize stuff */
  texture->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!texture->png_ptr)
    printf("[read_png_file] png_create_read_struct failed");

  texture->info_ptr = png_create_info_struct(texture->png_ptr);
  if (!texture->info_ptr)
    printf("[read_png_file] png_create_info_struct failed");

  if (setjmp(png_jmpbuf(texture->png_ptr)))
    printf("[read_png_file] Error during init_io");

  png_init_io(texture->png_ptr, fp);
  png_set_sig_bytes(texture->png_ptr, 8);

  png_read_info(texture->png_ptr, texture->info_ptr);

  texture->width = png_get_image_width(texture->png_ptr, texture->info_ptr);
  texture->height = png_get_image_height(texture->png_ptr, texture->info_ptr);
  texture->color_type = png_get_color_type(texture->png_ptr, texture->info_ptr);
  texture->bit_depth = png_get_bit_depth(texture->png_ptr, texture->info_ptr);

  texture->number_of_passes = png_set_interlace_handling(png_ptr);
  png_read_update_info(texture->png_ptr, texture->info_ptr);


  /* read file */
  if (setjmp(png_jmpbuf(texture->png_ptr)))
    printf("[read_png_file] Error during read_image");

  texture->row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * texture->height);
  for (y = 0; y < texture->height; y++)
    texture->row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(texture->png_ptr, texture->info_ptr));

  png_read_image(texture->png_ptr, texture->row_pointers);
  fclose(fp);

  texture->rgb = (unsigned char*) malloc(sizeof(unsigned char) * texture->width * texture->height * 3);
  for (y = 0; y < texture->height; y++) {
    for (int x = 0; x < texture->width; x++) {
      png_byte* row = texture->row_pointers[y];
      png_byte* ptr = &(row[x * 4]);
      texture->rgb[y * texture->width * 3 + x * 3 + 0] = ptr[0];
      texture->rgb[y * texture->width * 3 + x * 3 + 1] = ptr[1];
      texture->rgb[y * texture->width * 3 + x * 3 + 2] = ptr[2];
    }
  }
}

#endif
