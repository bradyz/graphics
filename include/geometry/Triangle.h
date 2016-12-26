#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <vector>
#include <glm/glm.hpp>

#include "geometry/Ray.h"
#include "geometry/BoundingBox.h"
#include "physics/RigidBody.h"
#include "physics/Intersection.h"

struct Triangle : RigidBody {
  std::vector<glm::vec3> points;

  Triangle (const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    points.push_back(a);
    points.push_back(b);
    points.push_back(c);
  }

  glm::vec3 barycenter () const {
    return (points[0] + points[1] + points[2]) / 3.0f;
  }

  virtual BoundingBox getBoundingBox () const;

  virtual bool intersects (const Ray& ray, Intersection& isect) const;
};

#endif
