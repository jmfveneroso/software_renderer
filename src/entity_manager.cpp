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

void EntityManager::CreateFrameBuffer(
  const std::string& name, int width, 
  int height, glm::vec2 top_left
) {
  std::shared_ptr<FrameBuffer> fb = std::make_shared<FrameBuffer>(width, height, top_left);
  frame_buffers_.insert(std::make_pair(name, fb));
}

void EntityManager::LoadShader(
  const std::string& name,  
  const std::string& vertex_file_path, 
  const std::string& fragment_file_path
) {
  Shader shader = Shader(name, vertex_file_path, fragment_file_path);
  shaders_.insert(std::make_pair(name, shader));
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
  CreateFrameBuffer("reflection", 1000, 750, glm::vec2( 0.0f,  0.0f));
  CreateFrameBuffer("refraction", 1000, 750, glm::vec2(-1.0f,  0.0f));
  CreateFrameBuffer("screen",     1000, 750, glm::vec2(-1.0f, -1.0f));

  // Create and compile our GLSL program from the shaders.
  Shader shader = Shader("default", "shaders/vshade_normals", "shaders/fshade_normals");
  shader.CreateUniform("DiffuseTextureSampler");
  shader.CreateUniform("NormalTextureSampler");
  shader.CreateUniform("SpecularTextureSampler");
  shader.CreateUniform("water_fog");
  shader.CreateUniform("LightPosition_worldspace");
  shader.CreateUniform("MVP");
  shader.CreateUniform("V");
  shader.CreateUniform("M");
  shader.CreateUniform("MV3x3");
  shader.CreateUniform("use_normals");
  shader.CreateUniform("plane");
  shaders_.insert(std::make_pair("default", shader));

  shader = Shader("terrain", "shaders/vshade_terrain", "shaders/fshade_terrain", "shaders/gshade_terrain");
  shader.CreateUniform("DiffuseTextureSampler");
  shader.CreateUniform("NormalTextureSampler");
  shader.CreateUniform("SpecularTextureSampler");
  shader.CreateUniform("RockTextureSampler");
  shader.CreateUniform("Rock2TextureSampler");
  shader.CreateUniform("SandTextureSampler");
  shader.CreateUniform("HeightMapSampler");
  shader.CreateUniform("NormalsSampler");
  shader.CreateUniform("ValidSampler");
  shader.CreateUniform("MVP");
  shader.CreateUniform("V");
  shader.CreateUniform("M");
  shader.CreateUniform("MV3x3");
  shader.CreateUniform("PlayerPosition");
  shader.CreateUniform("TILE_SIZE");
  shader.CreateUniform("CLIPMAP_SIZE");
  shader.CreateUniform("MAX_HEIGHT");
  shader.CreateUniform("buffer_top_left");
  shaders_.insert(std::make_pair("terrain", shader));

  shader = Shader("sky", "shaders/vshade_normals", "shaders/fshade_sky");
  shader.CreateUniform("DiffuseTextureSampler");
  shader.CreateUniform("NormalTextureSampler");
  shader.CreateUniform("SpecularTextureSampler");
  shader.CreateUniform("water_fog");
  shader.CreateUniform("LightPosition_worldspace");
  shader.CreateUniform("MVP");
  shader.CreateUniform("V");
  shader.CreateUniform("M");
  shader.CreateUniform("MV3x3");
  shader.CreateUniform("use_normals");
  shader.CreateUniform("plane");
  shaders_.insert(std::make_pair("sky", shader));

  shader = Shader("water", "shaders/vshade_water", "shaders/fshade_water");
  shader.CreateUniform("ReflectionTextureSampler");
  shader.CreateUniform("RefractionTextureSampler");
  shader.CreateUniform("dudvMap");
  shader.CreateUniform("normalMap");
  shader.CreateUniform("depthMap");
  shader.CreateUniform("LightPosition_worldspace");
  shader.CreateUniform("moveFactor");
  shader.CreateUniform("cameraPosition");

  shader.CreateUniform("HeightMapSampler");
  shader.CreateUniform("MVP");
  shader.CreateUniform("V");
  shader.CreateUniform("M");
  shader.CreateUniform("MV3x3");
  shader.CreateUniform("PlayerPosition");
  shader.CreateUniform("TILE_SIZE");
  shader.CreateUniform("CLIPMAP_SIZE");
  shader.CreateUniform("MAX_HEIGHT");
  shader.CreateUniform("buffer_top_left");
  shaders_.insert(std::make_pair("water", shader));

  LoadSolid(
    "sky", "res/skydome.obj", "textures/skydome.bmp", 
    "textures/normal.bmp", "textures/specular_orange.bmp", "sky"
  );

  GLuint diffuse_texture_id = LoadTexture("textures/water_dudv.bmp", "textures/water_dudv.bmp");
  GLuint normal_texture_id = LoadTexture("textures/water_normal.bmp", "textures/water_normal.bmp");

  auto water_it = shaders_.find("water");
  if (water_it == shaders_.end()) 
    throw "Shader water does not exist";

  std::shared_ptr<Water> water = std::make_shared<Water>(
    "res/water.obj", 
    water_it->second,
    diffuse_texture_id, 
    normal_texture_id, 
    frame_buffers_["reflection"]->GetTexture(),
    frame_buffers_["refraction"]->GetTexture(),
    frame_buffers_["refraction"]->GetDepthTexture()
  );
  entities_.insert(std::make_pair("water", water));


  // Procedural terrain.
  auto it = shaders_.find("terrain");
  if (it == shaders_.end()) 
    throw "Shader does not exist";

  // diffuse_texture_id = LoadTexture("diffuse_terrain", "textures/dirt.bmp");
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
    water
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
