#ifndef SPHERE_H
#define SPHERE_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "geometry/Plane.h"
#include "geometry/Ray.h"
#include "geometry/BoundingBox.h"
#include "physics/RigidBody.h"
#include "physics/Intersection.h"

struct Sphere : RigidBody {
  const double radius;

  Sphere (double r, const glm::vec3& p, double m=1.0) : RigidBody(p, m), radius(r) { }

  void step (const std::vector<glm::vec3>& forces) {
    position += RigidBody::stepOffset(forces);
  }

  glm::mat4 toWorld () const {
    glm::mat4 T = glm::translate(position);
    glm::mat4 S = glm::scale(glm::vec3(radius, radius, radius));
    return T * S;
  }

  virtual BoundingBox getBoundingBox () const {
    std::vector<glm::vec4> points;

    points.push_back(glm::vec4(position.x - radius, position.y, position.z, 1.0));
    points.push_back(glm::vec4(position.x + radius, position.y, position.z, 1.0));

    points.push_back(glm::vec4(position.x, position.y - radius, position.z, 1.0));
    points.push_back(glm::vec4(position.x, position.y + radius, position.z, 1.0));

    points.push_back(glm::vec4(position.x, position.y, position.z - radius, 1.0));
    points.push_back(glm::vec4(position.x, position.y, position.z + radius, 1.0));

    return BoundingBox(points);
  }

  bool intersects (const Sphere& other, Intersection& isect) const;
  bool intersects (const Plane& other, Intersection& isect) const;
  bool intersects (const BoundingBox& other, Intersection& isect) const;

  virtual bool intersects (const Ray& ray, Intersection& isect) const;
};

#endif
