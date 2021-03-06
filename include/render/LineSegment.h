#ifndef LINESEGMENT_H
#define LINESEGMENT_H

#include <vector>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "render/Program.h"
#include "geometry/BoundingBox.h"

struct LineSegmentProgram: public Program {
  GLint line_color_location = 0;

  LineSegmentProgram (glm::mat4* view_p, glm::mat4* proj_p) :
    Program(view_p, proj_p) {
  }

  virtual void setup();
  void draw (const std::vector<glm::vec4>& vertices,
             const std::vector<glm::uvec2>& segments,
             const glm::mat4& model_matrix, const glm::vec4& color);
  void drawLineSegment (const glm::vec3& u, const glm::vec3& v,
                        const glm::vec4& color);
  void drawBoundingBox (const BoundingBox& box, const glm::vec4& color);
  void drawAxis();
};

#endif
