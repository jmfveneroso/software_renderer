#ifndef _PLOT_HPP_
#define _PLOT_HPP_

#include <algorithm>
#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include <cstring>
#include <sstream>
#include <streambuf>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp> 
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include "renderer.hpp"
#include "text_editor.hpp"
#include "config.h"

namespace Sibyl {

enum ObjectType {
  OBJECT = 0,
  SCROLL,
  PLOT_2D
};

struct Object {
  unsigned int id;
  ObjectType type;
  glm::vec3 position_;
  GLfloat rotation_;
  bool highlighted = false;
  string mesh_name_;

  Object(
    unsigned int id,
    glm::vec3 position,
    GLfloat rotation,
    const string& mesh_name
  ) : id(id),
      position_(position),
      rotation_(rotation),
      mesh_name_(mesh_name) {
  }
};

class Plot : public Object {
 public:
  bool highlighted = false;
  bool collision = false;
  string filename;

  Plot(
    unsigned int id,
    string filename,
    glm::vec3 position,
    GLfloat rotation
  );
};

} // End of namespace.

#endif
