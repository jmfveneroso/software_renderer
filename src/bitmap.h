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

GLuint loadBMP_custom(const char* imagepath){
  printf("Reading image %s\n", imagepath);
  
  // Data read from the header of the BMP file
  unsigned char header[54];
  unsigned int dataPos;
  unsigned int imageSize;
  unsigned int width, height;
  // Actual RGB data
  unsigned char * data;
  
  // Open the file
  FILE * file = fopen(imagepath,"rb");
  if (!file){
  	printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath);
  	getchar();
  	return 0;
  }
  
  // Read the header, i.e. the 54 first bytes
  
  // If less than 54 bytes are read, problem
  if ( fread(header, 1, 54, file)!=54 ){ 
  	printf("Not a correct BMP file\n");
  	fclose(file);
  	return 0;
  }
  // A BMP files always begins with "BM"
  if ( header[0]!='B' || header[1]!='M' ){
  	printf("Not a correct BMP file\n");
  	fclose(file);
  	return 0;
  }
  // Make sure this is a 24bpp file
  if ( *(int*)&(header[0x1E])!=0  )         {printf("Not a correct BMP file\n");    fclose(file); return 0;}
  if ( *(int*)&(header[0x1C])!=24 )         {printf("Not a correct BMP file\n");    fclose(file); return 0;}
  
  // Read the information about the image
  dataPos    = *(int*)&(header[0x0A]);
  imageSize  = *(int*)&(header[0x22]);
  width      = *(int*)&(header[0x12]);
  height     = *(int*)&(header[0x16]);
  
  // Some BMP files are misformatted, guess missing information
  if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
  if (dataPos==0)      dataPos=54; // The BMP header is done that way
  
  // Create a buffer
  data = new unsigned char [imageSize];
  
  // Read the actual data from the file into the buffer
  fread(data,1,imageSize,file);
  
  // Everything is in memory now, the file can be closed.
  fclose (file);
  
  // Create one OpenGL texture
  GLuint textureID;
  glGenTextures(1, &textureID);
  
  // "Bind" the newly created texture : all future texture functions will modify this texture
  glBindTexture(GL_TEXTURE_2D, textureID);
  
  // Give the image to OpenGL
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
  
  // OpenGL has now copied the data. Free our own version
  delete [] data;
  
  // Poor filtering, or ...
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  
  // ... nice trilinear filtering ...
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  // ... which requires mipmaps. Generate them automatically.
  glGenerateMipmap(GL_TEXTURE_2D);
  
  // Return the ID of the texture we just created
  return textureID;
}

#endif
