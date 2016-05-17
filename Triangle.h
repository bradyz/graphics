#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <vector>
#include <glm/glm.hpp>

#include "BoundingBox.h"

struct Triangle {
  std::vector<glm::vec3> points;

  Triangle (const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    points.push_back(a);
    points.push_back(b);
    points.push_back(c);
  }

  virtual BoundingBox getBoundingBox () const {
    return box(points); 
  }
};

#endif
