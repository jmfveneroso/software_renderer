#include "terrain.hpp"

namespace Sibyl {

Terrain::Terrain(
  std::shared_ptr<Player> player,
  Shader shader, 
  Shader rock_shader, 
  Shader water_shader, 
  GLuint diffuse_texture_id, 
  GLuint normal_texture_id, 
  GLuint specular_texture_id,
  GLuint rock_texture_id,
  GLuint rock_2_texture_id,
  GLuint sand_texture_id,
  std::shared_ptr<Water> water
) : player_(player),
    height_map_(std::make_shared<HeightMap>()),
    shader_(shader),
    rock_shader_(rock_shader),
    water_shader_(water_shader),
    diffuse_texture_id_(diffuse_texture_id), 
    normal_texture_id_(normal_texture_id), 
    specular_texture_id_(specular_texture_id),
    rock_texture_id_(rock_texture_id),
    rock_2_texture_id_(rock_2_texture_id),
    sand_texture_id_(sand_texture_id),
    water_(water) {

  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    clipmaps_[i] = Clipmap(player_, height_map_, i + 1); 
  }

  CreateTerrainFeatures();
}

void Terrain::CreateTerrainFeatures() {
  for (int i = 0; i < 10; i++) {
    rocks_[i] = std::make_shared<Rock>(rock_shader_, rock_2_texture_id_);
  }

  for (int i = 0; i < 1; i++) {
    // int x = rand() % (HEIGHT_MAP_SIZE * TILE_SIZE) - HEIGHT_MAP_SIZE * TILE_SIZE / 2;
    // int z = rand() % (HEIGHT_MAP_SIZE * TILE_SIZE) - HEIGHT_MAP_SIZE * TILE_SIZE / 2;
    // int y = height_map_->GetHeight(x, z) * MAX_HEIGHT / 2 + 500.0f;
    // int r = 0.5f + 0.2f * float(rand() % 255) / 255;
    // int g = 0.5f + 0.2f * float(rand() % 255) / 255;
    // int b = 0.5f + 0.2f * float(rand() % 255) / 255;
    // int h_angle = 2 * PI * float(rand() % 100) / 100;
    // int v_angle = 2 * PI * float(rand() % 100) / 100;
    // float scale = 0.5f + 0.5 * float(rand() % 100) / 100;
    // features_.push_back(TerrainFeature(
    //   rand() % 10,                 // Id.
    //   glm::vec3(x, y, z),          // Position.
    //   glm::vec3(r, g, b),          // Color.
    //   glm::vec2(h_angle, v_angle), // Rotation.
    //   scale                        // Scale.
    // ));

    features_.push_back(TerrainFeature(
      rand() % 10,                 // Id.
      glm::vec3(-205892, 18500.5, 69348),          // Position.
      glm::vec3(1.0, 1.0, 1.0),          // Color.
      glm::vec2(0, 0), // Rotation.
      6.0                        // Scale.
    ));
  }
}

void Terrain::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  glUseProgram(shader_.program_id());

  // Textures.
  shader_.BindTexture("DiffuseTextureSampler", diffuse_texture_id_);
  shader_.BindTexture("NormalTextureSampler", normal_texture_id_);
  shader_.BindTexture("SpecularTextureSampler", specular_texture_id_);
  shader_.BindTexture("RockTextureSampler", rock_texture_id_);
  shader_.BindTexture("Rock2TextureSampler", rock_2_texture_id_);
  shader_.BindTexture("SandTextureSampler", sand_texture_id_);
  glUniform4fv(shader_.GetUniformId("plane"), 1, (float*) &clip_plane_);

  // Clipmap.
  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    clipmaps_[i].Update(player_->position());
  }

  bool first = true;
  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    clipmaps_[i].Render(player_->position(), &shader_, ProjectionMatrix, ViewMatrix, first);
    first = false;
  }

  for (auto& f : features_) {
    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0), f.position) * glm::scale(glm::vec3(f.scale));
    model_matrix *= glm::rotate(f.rotation.x, glm::vec3(0, 1, 0));
    model_matrix *= glm::rotate(f.rotation.y, glm::vec3(1, 0, 0));

    float distance = glm::distance(player_->position(), f.position);
    int lod = int(distance) / 25000;
    if (lod > 4) lod = 4;
    rocks_[f.id]->DrawShit(model_matrix, ProjectionMatrix, ViewMatrix, f.color, lod); 
  }

  shader_.Clear();

  if (draw_water_) {
    glUseProgram(water_shader_.program_id());

    water_shader_.BindTexture("ReflectionTextureSampler", water_->reflection_texture_id_);
    water_shader_.BindTexture("RefractionTextureSampler", water_->refraction_texture_id_);
    water_shader_.BindTexture("dudvMap", water_->diffuse_texture_id_);
    water_shader_.BindTexture("normalMap", water_->normal_texture_id_);
    water_shader_.BindTexture("depthMap", water_->refraction_depth_texture_id_);

    first = true;
    for (int i = 0; i < CLIPMAP_LEVELS; i++) {
      clipmaps_[i].RenderWater(player_->position(), &water_shader_, ProjectionMatrix, ViewMatrix, camera, first, water_);
      first = false;
    }

    water_shader_.Clear();
  }
}

void Terrain::Erode() {
  height_map_->CalculateErosion();
  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    clipmaps_[i].Clear();
  }
}

} // End of namespace.
