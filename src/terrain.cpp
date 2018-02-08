#include "terrain.hpp"

namespace Sibyl {

Terrain::Terrain(
  std::shared_ptr<Player> player,
  Shader shader, 
  Shader water_shader, 
  GLuint diffuse_texture_id, 
  GLuint normal_texture_id, 
  GLuint specular_texture_id,
  GLuint rock_texture_id,
  GLuint rock_2_texture_id,
  GLuint sand_texture_id,
  GLuint grass_normal_texture_id,
  std::shared_ptr<Water> water
) : player_(player),
    height_map_(std::make_shared<HeightMap>()),
    shader_(shader),
    water_shader_(water_shader),
    diffuse_texture_id_(diffuse_texture_id), 
    normal_texture_id_(normal_texture_id), 
    specular_texture_id_(specular_texture_id),
    rock_texture_id_(rock_texture_id),
    rock_2_texture_id_(rock_2_texture_id),
    sand_texture_id_(sand_texture_id),
    grass_normal_texture_id_(grass_normal_texture_id),
    water_(water) {

  for (int i = 0; i < CLIPMAP_LEVELS; i++) {
    clipmaps_[i] = Clipmap(player_, height_map_, i + 1); 
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
  shader_.BindTexture("GrassNormalTextureSampler", grass_normal_texture_id_);
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
