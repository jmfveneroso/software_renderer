#include "entity_manager.hpp"

namespace Sibyl {

EntityManager::EntityManager(
  std::shared_ptr<Player> player
) : player_(player) {
  Initialize();
}

GLuint EntityManager::LoadTexture(
  const std::string& name, 
  const std::string& texture_file_path
) {
  Texture texture = Texture(texture_file_path);
  textures_.insert(std::make_pair(name, texture.texture_id()));
  return texture.texture_id();
}

void EntityManager::LoadSolid(
  const std::string& name, 
  const std::string& obj_file_path,
  const std::string& texture_file_path,
  const std::string& normals_file_path,
  const std::string& specular_file_path,
  const std::string& shader
) {
  auto it = shaders_.find(shader);
  if (it == shaders_.end()) 
    throw "Shader does not exist";

  GLuint diffuse_texture_id = LoadTexture(texture_file_path, texture_file_path);
  GLuint normal_texture_id = LoadTexture(normals_file_path, normals_file_path);
  GLuint specular_texture_id = LoadTexture(specular_file_path, specular_file_path);

  std::shared_ptr<IEntity> entity = std::make_shared<Solid>(
    obj_file_path, 
    it->second,
    diffuse_texture_id, 
    normal_texture_id, 
    specular_texture_id
  );

  entities_.insert(std::make_pair(name, entity));
}

void EntityManager::Initialize() {
  // Create and compile our GLSL programs from the shaders.
  shaders_.insert(std::make_pair("default", Shader("default", "shaders/vshade_normals", "shaders/fshade_normals")));
  shaders_.insert(std::make_pair("terrain", Shader("terrain", "shaders/vshade_terrain", "shaders/fshade_terrain", "shaders/gshade_terrain")));
  shaders_.insert(std::make_pair("sky", Shader("sky", "shaders/vshade_sky", "shaders/fshade_sky")));
  shaders_.insert(std::make_pair("water", Shader("water", "shaders/vshade_water", "shaders/fshade_water")));
  shaders_.insert(std::make_pair("test", Shader("test", "shaders/vshade_test", "shaders/fshade_test", "shaders/gshade_test")));
  shaders_.insert(std::make_pair("static_screen", Shader("test", "shaders/vshade_static_screen", "shaders/fshade_static_screen")));

  frame_buffers_.insert(std::make_pair("reflection", std::make_shared<FrameBuffer>(shaders_.find("static_screen")->second, 600, 400)));
  frame_buffers_.insert(std::make_pair("refraction", std::make_shared<FrameBuffer>(shaders_.find("static_screen")->second, 600, 400)));
  frame_buffers_.insert(std::make_pair("screen",     std::make_shared<FrameBuffer>(shaders_.find("static_screen")->second, 600, 400)));
  // frame_buffers_.insert(std::make_pair("reflection", std::make_shared<FrameBuffer>(shaders_.find("static_screen")->second, 1000, 750)));
  // frame_buffers_.insert(std::make_pair("refraction", std::make_shared<FrameBuffer>(shaders_.find("static_screen")->second, 1000, 750)));
  // frame_buffers_.insert(std::make_pair("screen",     std::make_shared<FrameBuffer>(shaders_.find("static_screen")->second, 1000, 750)));

  // TestCube.
  entities_.insert(std::make_pair("cube", std::make_shared<Cube>(
    shaders_.find("test")->second,
    // frame_buffers_["screen"]->GetDepthTexture()
    frame_buffers_["refraction"]->GetDepthTexture()
  )));

  // Sky.
  sky_dome_ = std::make_shared<SkyDome>(
    player_,
    shaders_.find("sky")->second
  );
  entities_.insert(std::make_pair("sky", sky_dome_));


  // Water.
  GLuint diffuse_texture_id = LoadTexture("textures/water_dudv.bmp", "textures/water_dudv.bmp");
  GLuint normal_texture_id = LoadTexture("textures/water_normal.bmp", "textures/water_normal.bmp");

  auto water_it = shaders_.find("water");
  if (water_it == shaders_.end()) 
    throw "Shader water does not exist";

  water_ = std::make_shared<Water>(
    "res/water.obj", 
    water_it->second,
    diffuse_texture_id, 
    normal_texture_id, 
    frame_buffers_["reflection"]->GetTexture(),
    frame_buffers_["refraction"]->GetTexture(),
    frame_buffers_["refraction"]->GetDepthTexture()
  );
  entities_.insert(std::make_pair("water", water_));


  // Procedural terrain.
  auto it = shaders_.find("terrain");
  if (it == shaders_.end()) 
    throw "Shader does not exist";

  diffuse_texture_id = LoadTexture("diffuse_terrain", "textures/wild_grass.bmp");
  normal_texture_id = LoadTexture("normal_terrain", "textures/wild_grass_2.bmp");
  GLuint specular_texture_id = LoadTexture("specular_terrain", "textures/noise.bmp");
  GLuint rock_texture_id = LoadTexture("rock_terrain", "textures/rock_3.bmp");
  GLuint rock_2_texture_id = LoadTexture("rock_2_terrain", "textures/rock.bmp");
  GLuint sand_texture_id = LoadTexture("sand_terrain", "textures/sand.bmp");

  terrain_ = std::make_shared<Terrain>(
    player_,
    it->second,
    water_it->second,
    diffuse_texture_id, 
    normal_texture_id,
    specular_texture_id,
    rock_texture_id,
    rock_2_texture_id,
    sand_texture_id,
    water_
  );
  entities_.insert(std::make_pair("pro_terrain", terrain_));
}

std::shared_ptr<IEntity> EntityManager::GetEntity(const std::string& name) {
  auto it = entities_.find(name);
  if (it == entities_.end()) {
    std::cout << name << std::endl;
    throw "Entity does not exist";
  }
  return it->second;
}

std::shared_ptr<FrameBuffer> EntityManager::GetFrameBuffer(const std::string& name) {
  auto it = frame_buffers_.find(name);
  if (it == frame_buffers_.end()) {
    std::cout << name << std::endl;
    throw "Frame buffer does not exist";
  }
  return it->second;
}

void EntityManager::Clean() {
  for (auto it : shaders_)
    glDeleteProgram(it.second.program_id());

  for (auto it : textures_)
    glDeleteTextures(1, &it.second);

  for (auto it : entities_)
    it.second->Clean();
}

} // End of namespace.
