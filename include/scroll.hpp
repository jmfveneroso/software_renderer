#ifndef _SCROLL_HPP_
#define _SCROLL_HPP_

#include <algorithm>
#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include <cstring>
#include <sstream>
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
#include "wall_painting.hpp"
#include "shaders.h"
#include "config.h"

using namespace std;

namespace Sibyl {

class Scroll {

 public:
  Scroll() {}

  void Run();
};

} // End of namespace.

#endif
