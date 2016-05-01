#ifndef PHONG_H
#define PHONG_H

#include <vector>

#include <glm/glm.hpp>

#include "Program.h"

struct PhongProgram: public Program {
  PhongProgram (glm::mat4* view_p, glm::mat4* proj_p) :
    Program(view_p, proj_p) { }

  void setup();
  void draw (const std::vector<glm::vec4>& vertices, 
             const std::vector<glm::uvec3>& faces,
             const std::vector<glm::vec4>& normals,
             const glm::mat4& model);
};

#endif
