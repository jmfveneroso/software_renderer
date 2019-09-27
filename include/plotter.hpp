#ifndef _PLOTTER_HPP_
#define _PLOTTER_HPP_

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

class Plotter {
  shared_ptr<GameState> game_state_;
  shared_ptr<Renderer> renderer_;
  shared_ptr<TextEditor> text_editor_;

 public:
  Plotter(
    shared_ptr<GameState>,
    shared_ptr<Renderer>,
    shared_ptr<TextEditor>
  );

  void UpdatePlot(const string&, const string&);
};

} // End of namespace.

#endif
