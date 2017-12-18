#ifndef _ENTITY_MANAGER_HPP_
#define _ENTITY_MANAGER_HPP_

#include <iostream>
#include <string>
#include <map>
#include "shaders.h"
#include "entity.hpp"
#include "frame_buffer.hpp"

namespace Sibyl {

typedef std::map< std::string, std::shared_ptr<IEntity> > EntityMap;
typedef std::map<std::string, std::shared_ptr<FrameBuffer> > FrameBufferMap;
typedef std::map<std::string, GLuint> TextureMap;
typedef std::map<std::string, Shader> ShaderMap;

class EntityManager {
  TextureMap textures_;
  ShaderMap shaders_;
  FrameBufferMap frame_buffers_;
  EntityMap entities_;

  void LoadShader(const std::string&, const std::string&, const std::string&);
  GLuint LoadTexture(const std::string&, const std::string&);
  void LoadSolid(
    const std::string&, const std::string&, const std::string&,
    const std::string&, const std::string&, const std::string&
  );
  void CreateFrameBuffer(const std::string&, int, int, glm::vec2);

 public:
  EntityManager();

  EntityMap entities() { return entities_; };
  void Initialize();
  std::shared_ptr<IEntity> GetEntity(const std::string&);
  std::shared_ptr<FrameBuffer> GetFrameBuffer(const std::string&);
  void Clean();
};

} // End of namespace.

#endif
