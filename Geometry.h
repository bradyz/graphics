#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <glm/glm.hpp>

struct Geometry {
  std::vector<glm::vec4> vertices;
  std::vector<glm::uvec3> faces;
  std::vector<glm::vec4> normals;

  glm::mat4 toWorld;

  Geometry (std::vector<glm::vec4>& v, std::vector<glm::uvec3>& f) :
    vertices(v), faces(f) { }
};

#endif
