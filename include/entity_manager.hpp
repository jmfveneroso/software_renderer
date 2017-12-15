#ifndef _ENTITY_MANAGER_HPP_
#define _ENTITY_MANAGER_HPP_

#include <iostream>
#include <string>
#include <map>
#include "render_object.h"
#include "water.h"

namespace Sibyl {

struct DynamicTexture {
  int width, height;
  GLuint framebuffer_name;
  GLuint depth_render_buffer;
  GLuint rendered_texture;
  GLuint depth_texture;
  GLuint texture_id;
  GLuint quad_program_id;
  GLuint quad_vertex_buffer;
  GLuint top_left_id;
  GLuint depth_map_id;
  glm::vec2 top_left;
};

typedef std::map<std::string, RenderObject> EntityMap;
typedef std::map<std::string, FrameBuffer> FrameBufferMap;
typedef std::map<std::string, GLuint> TextureMap;
typedef std::map<std::string, GLuint> ShaderMap;

class EntityManager {
  TextureMap textures_;
  ShaderMap shaders_;
  FrameBufferMap frame_buffers_;
  EntityMap entities_;

  void LoadShader(const std::string&, const std::string&, const std::string&);
  void LoadTexture(const std::string&, const std::string&);
  void LoadEntity(
    const std::string&, const std::string&, const std::string&,
    const std::string&, const std::string&, const std::string&
  );
  void CreateFrameBuffer(const std::string&, int, int, glm::vec2);

 public:
  EntityManager();

  EntityMap entities() { return entities_; };
  void Initialize();
  RenderObject GetEntity(const std::string&);
  void Clean();
};

} // End of namespace.

#endif
