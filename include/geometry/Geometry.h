#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <glm/glm.hpp>
#include "helpers/RandomUtils.h"

struct Geometry {
  std::vector<glm::vec4> vertices;
  std::vector<glm::uvec3> faces;
  std::vector<glm::vec4> normals;

  glm::mat4 toWorld;

  Geometry (const std::vector<glm::vec4>& v, const std::vector<glm::uvec3>& f) :
      vertices(v), faces(f) {
    this->updateNormals();
  }

  void updateNormals() {
    this->normals = getVertexNormals(this->vertices, this->faces);
  }
};

#endif
