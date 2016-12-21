#ifndef QUAD_H
#define QUAD_H

#include <vector>
#include <glm/glm.hpp>

#include "geometry/BoundingBox.h"
#include "physics/RigidBody.h"

struct Quad : RigidBody {
  glm::vec3 points[4];

  Quad (const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3 &d) {
    points[0] = a;
    points[1] = b;
    points[2] = c;
    points[3] = d;
  }

  virtual BoundingBox getBoundingBox () const {
    BoundingBox box;
    for (int i = 0; i < 4; i++)
      box.add(points[i]);
    return box;
  }
};

#endif
