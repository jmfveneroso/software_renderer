#include "entity_manager.hpp"

namespace Sibyl {

EntityManager::EntityManager() {
  Initialize();
}

GLuint EntityManager::LoadTexture(
  const std::string& name, 
  const std::string& texture_file_path
) {
  GLuint texture_id = loadBMP_custom(texture_file_path.c_str());
  textures_.insert(std::make_pair(name, texture_id));
  return texture_id;
}

void EntityManager::CreateFrameBuffer(
  const std::string& name, int width, 
  int height, glm::vec2 top_left
) {
  FrameBuffer fb = FrameBuffer(width, height, top_left);
  frame_buffers_.insert(std::make_pair(name, fb));
  textures_.insert(std::make_pair(name + ".texture", fb.GetTexture()));
  textures_.insert(std::make_pair(name + ".depth_texture", fb.GetDepthTexture()));
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
  CreateFrameBuffer("reflection", 1000, 750, vec2( 0.0f,  0.0f));
  CreateFrameBuffer("refraction", 1000, 750, vec2(-1.0f,  0.0f));
  CreateFrameBuffer("screen",     1000, 750, vec2(-1.0f, -1.0f));

  // water = WaterRenderObject("res/water2.obj", reflection_water.GetTexture(), refraction_water.GetTexture(), "textures/water_dudv.bmp", "textures/water_normal.bmp", waterProgramID, refraction_water.GetDepthTexture());

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

  // LoadShader("sky", "shaders/vshade_normals", "shaders/fshade_sky");
  // LoadShader("water", "shaders/vshade_water", "shaders/fshade_water");

  LoadSolid(
    "terrain", "res/large_terrain.obj", "textures/large_terrain.bmp", 
    "textures/normal.bmp", "textures/specular_orange.bmp", "default"
  );

   LoadSolid(
    "sky", "res/skydome2.obj", "textures/skydome.bmp", 
    "textures/normal.bmp", "textures/specular_orange.bmp", "sky"
  );

  // LoadEntity(
  //   "sky", "res/skydome2.obj", "textures/skydome.bmp", 
  //   "textures/normal.bmp", "textures/specular_orange.bmp", "sky"
  // );

  // LoadEntity(
  //   "water", "res/water2.obj", reflection_water.GetTexture(),
  //   refraction_water.GetTexture(), "textures/water_dudv.bmp", "water",
  //   refraction_water.GetDepthTexture()
  // );
}

std::shared_ptr<IEntity> EntityManager::GetEntity(const std::string& name) {
  return entities_[name];
}

void EntityManager::Clean() {
  // for (auto shader_id : shaders_)
  //   glDeleteProgram(shaders_[shader_id]);

  // for (auto entity : entities_)
  //   entities_[entity].Clean();
}

} // End of namespace.
