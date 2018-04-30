#include "tree.hpp"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795f
#endif

namespace Sibyl {

Tree::Tree(Shader shader, 
  Shader twig_shader, 
  GLuint texture,
  GLuint twig_texture,
  GLuint twig_mask_texture
) : shader_(shader), twig_shader_(twig_shader), texture_(texture),
  twig_texture_(twig_texture), twig_mask_texture_(twig_mask_texture) {
  position_ = glm::vec3(-187962, 18000, 108584);

  Generate();
}

Branch::~Branch() {
  delete child0;
  delete child1;
  delete[] root_ring;
  delete[] ring0;
  delete[] ring1;
  delete[] ring2;
}

Branch::Branch() {
  root_ring = 0;
  ring0 = 0;
  ring1 = 0;
  ring2 = 0;
  child0 = 0;
  child1 = 0;
  length = 1;
  trunk_type = 0;
  radius = 0;
  tangent = glm::vec3(0);
  end = 0;
  head = glm::vec3(0);
  parent = 0;
}

Branch::Branch(glm::vec3 head, Branch* parent) : Branch() {
  this->head = head;
  this->parent = parent;
}

float Tree::GetRandom(float fixed) {
  if (!fixed) fixed = (float) r_seed_++;
  return std::abs(std::cos(fixed + fixed * fixed));
}

glm::vec3 Tree::MirrorBranch(glm::vec3 a_vec, glm::vec3 a_norm) {
  glm::vec3 v = glm::cross(a_norm, glm::cross(a_vec, a_norm));
  float s = branch_factor_ * dot(v, a_vec);
  return glm::vec3(
    a_vec.x - v.x * s,
    a_vec.y - v.y * s,
    a_vec.z - v.z * s
  );
}

glm::vec3 VecAxisAngle(glm::vec3 aVec, glm::vec3 aAxis, float aAngle) {
  float cosr = std::cos(aAngle);
  float sinr = std::sin(aAngle);
  return (aVec * cosr + cross(aAxis, aVec) * sinr) +
         (aAxis * glm::dot(aAxis, aVec) * (1 - cosr));
}

glm::vec3 ScaleInDirection(glm::vec3 aVector, glm::vec3 aDirection, float aScale) {
  float currentMag = glm::dot(aVector, aDirection);
  glm::vec3 change = aDirection * (currentMag * aScale - currentMag);
  return aVector + change;
}

void Tree::Split(Branch* branch, int level, int steps, int aL1, int aL2) {
  int r_level = levels_ - level;

  glm::vec3 po;
  if (branch->parent) {
    po = branch->parent->head;
  } else {
    po = glm::vec3( 0, 0, 0 );
    branch->trunk_type = 1;
  }

  glm::vec3 so = branch->head;
  glm::vec3 dir = glm::normalize(so - po);

  glm::vec3 a = glm::vec3(dir.z, dir.x, dir.y);
  glm::vec3 normal = glm::cross(dir, a);
  glm::vec3 tangent = glm::cross(dir, normal);
  float r = GetRandom(r_level * 10 + aL1 * 5.0f + aL2 + seed_);

  glm::vec3 adj = (normal * r) + tangent * (1 - r);
  if (r > 0.5) adj = -adj;

  float clump = (clump_max_ - clump_min_) * r + clump_min_;
  glm::vec3 newdir = glm::normalize((adj * (1 - clump)) + (dir * clump));

  glm::vec3 newdir2 = MirrorBranch(newdir, dir);
  if (r > 0.5) {
    glm::vec3 tmp = newdir;
    newdir = newdir2;
    newdir2 = tmp;
  }

  if (steps > 0) {
    float angle = steps / (float) tree_steps_ * 2 * M_PI * twist_rate_;
    a = { std::sin(angle), r, std::cos(angle) };
    newdir2 = glm::normalize(a);
  }

  float grow_amount = level * level / (float)(levels_ * levels_) * grow_amount_;
  float drop_amount = r_level * drop_amount_;
  float sweep_amount = r_level * sweep_amount_;
  a = { sweep_amount, drop_amount + grow_amount, 0 };
  newdir = glm::normalize(newdir + a);
  newdir2 = glm::normalize(newdir2 + a);

  glm::vec3 head0 = so + newdir * branch->length;
  glm::vec3 head1 = so + newdir2 * branch->length;

  branch->child0 = new Branch(head0, branch);
  branch->child1 = new Branch(head1, branch);
  branch->child0->length = pow(branch->length, length_falloff_power_) * length_falloff_factor_;
  branch->child1->length = pow(branch->length, length_falloff_power_) * length_falloff_factor_;

  if (level > 0) {
    if (steps > 0) {
      a = {
        (r - 0.5f) * 2 * trunk_kink_,
        climb_rate_,
        (r - 0.5f) * 2 * trunk_kink_
      };
      branch->child0->head = branch->head + a;
      branch->child0->trunk_type = 1;
      branch->child0->length = branch->length * taper_rate_;
      Split(branch->child0, level, steps - 1, aL1 + 1, aL2);
    } else {
      Split(branch->child0, level - 1, 0, aL1 + 1, aL2);
    }
    Split(branch->child1, level - 1, 0, aL1, aL2 + 1);
  }
}

void Tree::CreateForks(Branch* branch, float radius) {
  if (!branch) branch = root_;
  if (!radius) radius = max_radius_;
  branch->radius = radius;

  if (radius > branch->length) radius = branch->length;

  int segments = segments_;

  float segmentAngle = M_PI * 2 / (float) segments;

  glm::vec3 v;

  // Trunk base.
  if (!branch->parent) {
    branch->root_ring = new int[segments];

    glm::vec3 axis = glm::vec3( 0, 1, 0 );
    for (int i = 0; i < segments; i++) {
      glm::vec3 left = glm::vec3( -1, 0, 0 );
      glm::vec3 vec = VecAxisAngle(left, axis, -segmentAngle * i);
      branch->root_ring[i] = vert_count_++;
      branch->root_ring[i] = vertices_.size();
 
      v = 5000.0f * (vec * (radius / radius_falloff_rate_));
      vertices_.push_back(v); normals_.push_back(glm::normalize(v - branch->head));
      uvs_.push_back(glm::vec2( i / (float) segments, 0 ));
    }
  }

  // Intersection.
  if (branch->child0) {
    glm::vec3 axis;
    if (branch->parent) {
      axis = glm::normalize(branch->head - branch->parent->head);
    } else {
      axis = glm::normalize(branch->head); // 0, 1, 0.
    }

    glm::vec3 axis1 = glm::normalize(branch->head - branch->child0->head);
    glm::vec3 axis2 = glm::normalize(branch->head - branch->child1->head);
    glm::vec3 tangent = glm::normalize(glm::cross(axis1, axis2));
    branch->tangent = tangent;

    glm::vec3 axis3 = glm::normalize(glm::cross(tangent, glm::normalize(-axis1 + -axis2)));
    glm::vec3 dir = { axis2.x, 0, axis2.z };
    glm::vec3 centerloc = branch->head + dir * (-max_radius_ / 2.0f);

    branch->ring0 = new int[segments];
    branch->ring1 = new int[segments];
    branch->ring2 = new int[segments];

    int ring0count = 0;
    int ring1count = 0;
    int ring2count = 0;

    float scale = radius_falloff_rate_;
    if (branch->child0->trunk_type || branch->trunk_type) {
      scale = 1.0f / taper_rate_;
    }

    int linch0 = vert_count_++;
    branch->ring0[ring0count++] = linch0;
    branch->ring2[ring2count++] = linch0;

    v = 5000.0f * (centerloc + tangent * radius * scale);
    vertices_.push_back(v); normals_.push_back(glm::normalize(v - branch->head));
    uvs_.push_back(glm::vec2( 0.5, branch->length * v_multiplier_ ));

    int start = vert_count_ - 1;
    glm::vec3 d1 = VecAxisAngle(tangent, axis2, 1.57f);
    glm::vec3 d2 = glm::normalize(glm::cross(tangent, axis));
    float s = 1 / glm::dot(d1, d2);
    for (int i = 1; i < segments / 2; i++) {
      glm::vec3 vec = VecAxisAngle(tangent, axis2, segmentAngle * i);
      branch->ring0[ring0count++] = start + i;
      branch->ring2[ring2count++] = start + i;
      vec = ScaleInDirection(vec, d2, s);
      vert_count_++;

      v = 5000.0f * (centerloc + vec * radius * scale);
      vertices_.push_back(v); normals_.push_back(glm::normalize(v - branch->head));
      uvs_.push_back(glm::vec2( i / (float) segments, branch->length * v_multiplier_ ));
    }

    int linch1 = vert_count_++;
    branch->ring0[ring0count++] = linch1;
    branch->ring1[ring1count++] = linch1;

    v = 5000.0f * (centerloc + tangent * -radius * scale);
    vertices_.push_back(v); normals_.push_back(glm::normalize(v - branch->head));
    uvs_.push_back(glm::vec2( 0.5, branch->length * v_multiplier_ ));

    for (int i = segments / 2 + 1; i < segments; i++) {
      glm::vec3 vec = VecAxisAngle(tangent, axis1, segmentAngle * i);
      branch->ring0[ring0count++] = vert_count_;
      branch->ring1[ring1count++] = vert_count_++;

      v = 5000.0f * (centerloc + vec * radius * scale);
      vertices_.push_back(v); normals_.push_back(glm::normalize(v - branch->head));
      uvs_.push_back(glm::vec2( i / (float) segments, branch->length * v_multiplier_ ));
    }

    branch->ring1[ring1count++] = linch0; 
    branch->ring2[ring2count++] = linch1; 
    start = vert_count_ - 1;
    for (int i = 1; i < segments / 2; i++) {
      glm::vec3 vec = VecAxisAngle(tangent, axis3, segmentAngle * i);
      branch->ring1[ring1count++] = start + i;
      branch->ring2[ring2count++] = start + (segments / 2 - i);
      v = vec * radius * scale;
      vert_count_++;

      v = 5000.0f * (centerloc + v);
      vertices_.push_back(v); normals_.push_back(glm::normalize(v - branch->head));
      uvs_.push_back(glm::vec2( i / (float) segments, branch->length * v_multiplier_ ));
    }

    // Print branch ring.
    // for (int i = 0; i < ring0count; i++) {
    //   indices_.push_back(branch->ring0[i]);
    //   if (i < ring0count - 1) 
    //     indices_.push_back(branch->ring0[i + 1]);
    //   else
    //     indices_.push_back(branch->ring0[0]);
    // }

    float radius0 = 1 * radius * radius_falloff_rate_;
    float radius1 = 1 * radius * radius_falloff_rate_;
    if (branch->child0->trunk_type) {
      radius0 = radius * taper_rate_;
    }

    CreateForks(branch->child0, radius0);
    CreateForks(branch->child1, radius1);

  // Twig.
  } else {
    branch->end = vert_count_++;

    v = 5000.0f * (branch->head);
    vertices_.push_back(v); normals_.push_back(glm::normalize(v - branch->head));
    uvs_.push_back(glm::vec2( 0.5, branch->length * v_multiplier_ ));
  }
}

void Tree::CreateTwigs(Branch* branch) {
  if (!branch) branch = root_;

  if (!branch->child0) {
    glm::vec3 tangent = glm::normalize(glm::cross(branch->parent->child0->head - branch->parent->head, branch->parent->child1->head - branch->parent->head));
    glm::vec3 binormal = glm::normalize(branch->head - branch->parent->head);

    int v1 = twig_vert_count_++;
    twig_vertices_.push_back(5000.0f * ((branch->head + tangent * twig_scale_ ) + binormal * (twig_scale_ * 2 - branch->length)));
    int v2 = twig_vert_count_++;
    twig_vertices_.push_back(5000.0f * ((branch->head + tangent * -twig_scale_) + binormal * (twig_scale_ * 2 - branch->length)));
    int v3 = twig_vert_count_++;
    twig_vertices_.push_back(5000.0f * ((branch->head + tangent * -twig_scale_) + binormal * -branch->length));
    int v4 = twig_vert_count_++;
    twig_vertices_.push_back(5000.0f * ((branch->head + tangent * twig_scale_ ) + binormal * -branch->length));

    int v8 = twig_vert_count_++;
    twig_vertices_.push_back(5000.0f * ((branch->head + tangent * twig_scale_ ) + binormal * (twig_scale_ * 2 - branch->length)));
    int v7 = twig_vert_count_++;
    twig_vertices_.push_back(5000.0f * ((branch->head + tangent * -twig_scale_) + binormal * (twig_scale_ * 2 - branch->length)));
    int v6 = twig_vert_count_++;
    twig_vertices_.push_back(5000.0f * ((branch->head + tangent * -twig_scale_) + binormal * -branch->length));
    int v5 = twig_vert_count_++;
    twig_vertices_.push_back(5000.0f * ((branch->head + tangent * twig_scale_ ) + binormal * -branch->length));

    twig_indices_.push_back(v1); twig_indices_.push_back(v2); twig_indices_.push_back(v3);

    twig_indices_.push_back(v4); twig_indices_.push_back(v1); twig_indices_.push_back(v3);

    twig_indices_.push_back(v6); twig_indices_.push_back(v7); twig_indices_.push_back(v8);

    twig_indices_.push_back(v6); twig_indices_.push_back(v8); twig_indices_.push_back(v5);

    glm::vec3 normal  = glm::normalize(glm::cross(twig_vertices_[v1] - twig_vertices_[v3], twig_vertices_[v2] - twig_vertices_[v3]));
    glm::vec3 normal2 = glm::normalize(glm::cross(twig_vertices_[v7] - twig_vertices_[v6], twig_vertices_[v8] - twig_vertices_[v6]));

    twig_uvs_.push_back(glm::vec2( 0, 0 ));
    twig_uvs_.push_back(glm::vec2( 1, 0 ));
    twig_uvs_.push_back(glm::vec2( 1, 1 ));
    twig_uvs_.push_back(glm::vec2( 0, 1 ));
    twig_uvs_.push_back(glm::vec2( 0, 0 ));
    twig_uvs_.push_back(glm::vec2( 1, 0 ));
    twig_uvs_.push_back(glm::vec2( 1, 1 ));
    twig_uvs_.push_back(glm::vec2( 0, 1 ));

    twig_normals_.push_back(normal);
    twig_normals_.push_back(normal);
    twig_normals_.push_back(normal);
    twig_normals_.push_back(normal);
    twig_normals_.push_back(normal2);
    twig_normals_.push_back(normal2);
    twig_normals_.push_back(normal2);
    twig_normals_.push_back(normal2);
  } else {
    CreateTwigs(branch->child0);
    CreateTwigs(branch->child1);
  }
}

void Tree::DoFaces(Branch* branch) {
  if (!branch) branch = root_;

  int segments = segments_;

  // Root.
  if (!branch->parent) {
    glm::vec3 tangent = glm::normalize(glm::cross(branch->child0->head - branch->head, branch->child1->head - branch->head));
    glm::vec3 normal = glm::normalize(branch->head);
    glm::vec3 left = glm::vec3( -1, 0, 0 );
    float angle = std::acos(glm::dot(tangent, left));
    if (glm::dot(glm::cross(left, tangent), normal) > 0) {
      angle = 2 * M_PI - angle;
    }

    int seg_offset = (int) floor(0.5f + (angle / M_PI / 2 * segments));
    for (int i = 0; i < segments; i++) {
      int v1 = branch->ring0[i];
      int v2 = branch->root_ring[(i + seg_offset + 1) % segments];
      int v3 = branch->root_ring[(i + seg_offset) % segments];
      int v4 = branch->ring0[(i + 1) % segments];

      faces_.push_back(glm::ivec3( v1, v4, v3 ));
      faces_.push_back(glm::ivec3( v4, v2, v3 ));

      indices_.push_back(v1); indices_.push_back(v4); indices_.push_back(v3);
      indices_.push_back(v4); indices_.push_back(v2); indices_.push_back(v3);

      // uvs_[(i + seg_offset) % segments] = glm::vec2( i / (float) segments, 0 );

      // float len = glm::length(vert_[branch->ring0[i]] - vert_[branch->root_ring[(i + seg_offset) % segments]]) * v_multiplier_;
      // uv_[branch_->ring0[i]] = glm::vec2( i / (float)segments, len );
      // uv_[branch_->ring2[i]] = glm::vec2( i / (float)segments, len );
    }
  }

  if (branch->child0->ring0 == 0) {
    for (int i = 0; i < segments; i++) {
      int v1 = branch->child0->end;
      int v2 = branch->ring1[(i + 1) % segments];
      int v3 = branch->ring1[i];
      int v4 = branch->ring2[(i + 1) % segments];
      int v5 = branch->ring2[i];
      
      faces_.push_back(glm::ivec3(
        branch->child0->end,
        branch->ring1[(i + 1) % segments],
        branch->ring1[i]
      ));

      faces_.push_back(glm::ivec3(
        branch->child0->end,
        branch->ring2[(i + 1) % segments],
        branch->ring2[i]
      ));

      indices_.push_back(v1); indices_.push_back(v2); indices_.push_back(v3);
      indices_.push_back(v1); indices_.push_back(v4); indices_.push_back(v5);
    }
  } else {
    int seg_offset0 = -1, seg_offset1 = -1;
    float match0, match1;

    glm::vec3 v1 = glm::normalize(vertices_[branch->ring1[0]] - branch->head);
    glm::vec3 v2 = glm::normalize(vertices_[branch->ring2[0]] - branch->head);

    v1 = ScaleInDirection(v1, glm::normalize(branch->child0->head - branch->head), 0);
    v2 = ScaleInDirection(v2, glm::normalize(branch->child1->head - branch->head), 0);

    for (int i = 0; i < segments; i++) {
      glm::vec3 d = glm::normalize(vertices_[branch->child0->ring0[i]] - branch->child0->head);
      float l = glm::dot(d, v1);
      if (seg_offset0 == -1 || l > match0) {
        match0 = l;
        seg_offset0 = segments - i;
      }

      d = glm::normalize(vertices_[branch->child1->ring0[i]] - branch->child1->head);
      l = glm::dot(d, v2);
      if (seg_offset1 == -1 || l > match1) {
        match1 = l;
        seg_offset1 = segments - i;
      }
    }

    // float uv_scale = max_radius_ / branch->radius;

    for (int i = 0; i < segments; i++) {
      int v1 = branch->child0->ring0[i];
      int v2 = branch->ring1[(i + seg_offset0 + 1) % segments];
      int v3 = branch->ring1[(i + seg_offset0) % segments];
      int v4 = branch->child0->ring0[(i + 1) % segments];

      faces_.push_back(glm::ivec3(v1, v4, v3));
      faces_.push_back(glm::ivec3(v4, v2, v3));

      indices_.push_back(v1); indices_.push_back(v4); indices_.push_back(v3);
      indices_.push_back(v4); indices_.push_back(v2); indices_.push_back(v3);

      v1 = branch->child1->ring0[i];
      v2 = branch->ring2[(i + seg_offset1 + 1) % segments];
      v3 = branch->ring2[(i + seg_offset1) % segments];
      v4 = branch->child1->ring0[(i + 1) % segments];

      faces_.push_back(glm::ivec3(v1, v2, v3));
      faces_.push_back(glm::ivec3(v1, v4, v2));

      indices_.push_back(v1); indices_.push_back(v2); indices_.push_back(v3);
      indices_.push_back(v1); indices_.push_back(v4); indices_.push_back(v2);

      // float len1 = length_n(sub(mVert[aBranch->mChild0->mRing0[i]], mVert[aBranch->mRing1[(i + segOffset0) % segments]])) * UVScale;
      // fvec2 uv1 = mUV[aBranch->mRing1[(i + segOffset0 - 1) % segments]];

      // mUV[aBranch->mChild0->mRing0[i]] = { uv1.u, uv1.v + len1 * mProperties.mVMultiplier };
      // mUV[aBranch->mChild0->mRing2[i]] = { uv1.u, uv1.v + len1 * mProperties.mVMultiplier };

      // float len2 = length_n(sub(mVert[aBranch->mChild1->mRing0[i]], mVert[aBranch->mRing2[(i + segOffset1) % segments]])) * UVScale;
      // fvec2 uv2 = mUV[aBranch->mRing2[(i + segOffset1 - 1) % segments]];

      // mUV[aBranch->mChild1->mRing0[i]] = { uv2.u, uv2.v + len2 * mProperties.mVMultiplier };
      // mUV[aBranch->mChild1->mRing2[i]] = { uv2.u, uv2.v + len2 * mProperties.mVMultiplier };
    }

    DoFaces(branch->child0);
    DoFaces(branch->child1);
  }
}

void Tree::PrintBranch(Branch* b, int index) {
  glm::vec3 v = b->head * 5000.0f;
  vertices_.push_back(b->head * 5000.0f);
  
  int new_index = vertices_.size() - 1;

  indices_.push_back(index);
  indices_.push_back(new_index);

  if (b->child0) {
    PrintBranch(b->child0, new_index);
  }
  if (b->child1) {
    PrintBranch(b->child1, new_index);
  }
}

void Tree::Generate() {
  glGenBuffers(1, &vertex_buffer_);
  glGenBuffers(1, &uv_buffer_);
  glGenBuffers(1, &normal_buffer_);
  glGenBuffers(1, &element_buffer_);

  glGenBuffers(1, &twig_vertex_buffer_);
  glGenBuffers(1, &twig_uv_buffer_);
  glGenBuffers(1, &twig_normal_buffer_);
  glGenBuffers(1, &twig_element_buffer_);

  glm::vec3 start = { 0, trunk_length_, 0 };
  root_ = new Branch(start, 0);
  root_->length = initial_branch_length_;
  Split(root_, levels_, tree_steps_);
  CreateForks(0, 0);
  CreateTwigs(0);
  DoFaces(0);

  // vertices_.push_back(glm::vec3(0, 0, 0));
  // PrintBranch(root_, vertices_.size() - 1);

  // Trunk.
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, uvs_.size() * sizeof(glm::vec2), &uvs_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
  glBufferData(GL_ARRAY_BUFFER, normals_.size() * sizeof(glm::vec3), &normals_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    indices_.size() * sizeof(unsigned int), 
    &indices_[0], 
    GL_STATIC_DRAW
  );

  // Twigs.
  glBindBuffer(GL_ARRAY_BUFFER, twig_vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, twig_vertices_.size() * sizeof(glm::vec3), &twig_vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, twig_uv_buffer_);
  glBufferData(GL_ARRAY_BUFFER, twig_uvs_.size() * sizeof(glm::vec2), &twig_uvs_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, twig_normal_buffer_);
  glBufferData(GL_ARRAY_BUFFER, twig_normals_.size() * sizeof(glm::vec3), &twig_normals_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, twig_element_buffer_);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    twig_indices_.size() * sizeof(unsigned int), 
    &twig_indices_[0], 
    GL_STATIC_DRAW
  );
}

void Tree::Draw(glm::mat4 ProjectionMatrix, glm::mat4 ViewMatrix, glm::vec3 camera) {
  glUseProgram(shader_.program_id());

  glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), position_);
  glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
  glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

  glUniformMatrix4fv(shader_.GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(shader_.GetUniformId("V"),     1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix3fv(shader_.GetUniformId("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

  shader_.BindBuffer(vertex_buffer_, 0, 3);
  shader_.BindBuffer(uv_buffer_, 1, 2);
  shader_.BindBuffer(normal_buffer_, 2, 3);
  shader_.BindTexture("BarkTextureSampler", texture_);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, (void*) 0);
  shader_.Clear();

  // Draw twigs.
  glUseProgram(twig_shader_.program_id());
  glEnable(GL_BLEND); 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUniformMatrix4fv(twig_shader_.GetUniformId("MVP"),   1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(twig_shader_.GetUniformId("M"),     1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(twig_shader_.GetUniformId("V"),     1, GL_FALSE, &ViewMatrix[0][0]);
  glUniformMatrix3fv(twig_shader_.GetUniformId("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
  twig_shader_.BindBuffer(twig_vertex_buffer_, 0, 3);
  twig_shader_.BindBuffer(twig_uv_buffer_, 1, 2);
  twig_shader_.BindBuffer(twig_normal_buffer_, 2, 3);
  twig_shader_.BindTexture("TwigTextureSampler", twig_texture_);
  twig_shader_.BindTexture("TwigMaskTextureSampler", twig_mask_texture_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, twig_element_buffer_);
  for (int i = 0; i < twig_indices_.size() / 3; i++) {
    int start = i * 3;
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*) (start * sizeof(unsigned int)));
  }
  twig_shader_.Clear();
  glDisable(GL_BLEND);
}

} // End of namespace.
