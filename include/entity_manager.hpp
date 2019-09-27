#ifndef _ENTITY_MANAGER_HPP_
#define _ENTITY_MANAGER_HPP_

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
#include "building.hpp"
#include "terrain.hpp"
#include "plotter.hpp"
#include "shaders.h"
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

struct Scroll : public Object {
  string filename;

  Scroll(
    unsigned int id,
    glm::vec3 position,
    GLfloat rotation,
    const std::string& filename
  ) : Object(id, position, rotation, "scroll"),
      filename(filename) {
  }
};

struct Plot : public Object {
 public:
  bool collision = false;
  string filename;

  Plot(
    unsigned int id,
    string filename,
    glm::vec3 position,
    GLfloat rotation
  ) : Object(id, position, rotation, "2d_plot"),
      filename(filename) {
  }
};

class EntityManager {
  int create_object_ = -1;
  unsigned int active_object_id_ = 0;
  unsigned int id_counter = 1;
  shared_ptr<GameState> game_state_;
  shared_ptr<Renderer> renderer_;
  shared_ptr<TextEditor> text_editor_;
  shared_ptr<Building> building_;
  shared_ptr<Terrain> terrain_;
  shared_ptr<Plotter> plotter_;

  vector<Object> objects_;
  vector<Scroll> scrolls_;
  vector<Plot> plots_;

  int CreatePlot(const string&, vec3, GLfloat);
  string GetNewFilename(const string&);
  void Init();
  void Load(const string&);
  void Save(const string&);

 public:
  EntityManager(
    shared_ptr<GameState>, 
    shared_ptr<Renderer>, 
    shared_ptr<TextEditor>, 
    shared_ptr<Building>,
    shared_ptr<Plotter>
  );

  void Update();
  void Interact(bool);
  void Draw();
  void DrawCreateObject();
  void Collide(glm::vec3&, glm::vec3, bool&, glm::vec3&);
  void set_terrain(shared_ptr<Terrain> terrain) { terrain_ = terrain; } 
};

} // End of namespace.

#endif
