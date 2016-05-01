#ifndef WIRE_H
#define WIRE_H

#include <vector>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "Program.h"

struct WireProgram : public Program {
  GLint line_color_location = 0; 

  WireProgram (glm::mat4* view_p, glm::mat4* proj_p) :
    Program(view_p, proj_p) { }

  void setup();
  void draw (const std::vector<glm::vec4>& vertices, 
             const std::vector<glm::uvec3>& faces, 
             const glm::mat4& model, const glm::vec4& color);
};

#endif
