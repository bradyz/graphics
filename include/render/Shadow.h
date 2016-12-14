#ifndef SHADOW_H
#define SHADOW_H

#include <vector>

#include <glm/glm.hpp>

#include <GL/glew.h>

#include "render/Program.h"

struct ShadowProgram: public Program {
  GLint shadow_matrix_location = 0;

  glm::mat4 shadow_matrix;

  ShadowProgram (glm::mat4* view_p, glm::mat4* proj_p) :
    Program(view_p, proj_p) {
  }

  virtual void setup();
  void draw (const std::vector<glm::vec4>& vertices,
             const std::vector<glm::uvec3>& faces,
             const glm::mat4& model);
};

#endif
