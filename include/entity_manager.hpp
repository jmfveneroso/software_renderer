#ifndef _ENTITY_MANAGER_HPP_
#define _ENTITY_MANAGER_HPP_

#include <iostream>
#include <string>
#include <map>
#include <memory>
#include "shaders.h"
#include "entity.hpp"
#include "terrain.hpp"
#include "texture.hpp"
#include "sky_dome.hpp"
#include "frame_buffer.hpp"

namespace Sibyl {

typedef std::map< std::string, std::shared_ptr<IEntity> > EntityMap;
typedef std::map< std::string, std::shared_ptr<FrameBuffer> > FrameBufferMap;
typedef std::map<std::string, GLuint> TextureMap;
typedef std::map<std::string, Shader> ShaderMap;

class EntityManager {
  std::shared_ptr<Player> player_;
  TextureMap textures_;
  ShaderMap shaders_;
  FrameBufferMap frame_buffers_;
  EntityMap entities_;
  std::shared_ptr<Terrain> terrain_;
  std::shared_ptr<Water> water_;
  std::shared_ptr<SkyDome> sky_dome_;

  GLuint LoadTexture(const std::string&, const std::string&);
  void LoadSolid(
    const std::string&, const std::string&, const std::string&,
    const std::string&, const std::string&, const std::string&
  );

 public:
  EntityManager(
    std::shared_ptr<Player> player
  );

  EntityMap entities() { return entities_; };
  void Initialize();
  std::shared_ptr<Terrain> GetTerrain() { return terrain_; }
  std::shared_ptr<Water> GetWater() { return water_; }
  std::shared_ptr<IEntity> GetEntity(const std::string&);
  std::shared_ptr<FrameBuffer> GetFrameBuffer(const std::string&);
  void Clean();
};

} // End of namespace.

#endif
