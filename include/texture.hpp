#ifndef _TEXTURE_HPP_
#define _TEXTURE_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <GL/glew.h>

class Texture {
  GLuint texture_id_;

  void LoadBitmap(const char*);

 public:
  Texture(const std::string&);

  GLuint texture_id() { return texture_id_; }
};

#endif
