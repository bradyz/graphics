#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <vector>
#include <glm/glm.hpp>

#include "geometry/BoundingBox.h"
#include "geometry/Ray.h"

#include "physics/Intersection.h"

const float DT = 0.01f;

struct RigidBody {
  glm::vec3 prev_acceleration;
  glm::vec3 acceleration;
  glm::vec3 velocity;
  glm::vec3 position;
  glm::vec3 force;

  glm::vec4 color;

  float mass;

  RigidBody () : prev_acceleration(), acceleration(), velocity(), mass(1.0) { }

  RigidBody (const glm::vec3 &p, double m=1.0, const glm::vec4 &c=glm::vec4(1.0, 0.0, 0.0, 1.0)) :
    position(p), mass(m), color(c), velocity() { }

  void applyForce (const glm::vec3 &force) {
    this->force += force;
  }

  glm::vec3 stepOffset (const std::vector<glm::vec3>& forces) {
    glm::vec3 total_force = this->force;

    for (const glm::vec3& force: forces)
      total_force += force;

    acceleration = total_force / mass;
    // velocity += (prev_acceleration + acceleration) / 2.0f * DT;
    velocity += acceleration * DT;
    // velocity = 0.95f * glm::clamp(velocity, -25.0f, 25.0f);
    prev_acceleration = acceleration;

    this->force = glm::vec3(0.0, 0.0, 0.0);

    return DT * velocity;
  }

  virtual BoundingBox getBoundingBox () const = 0;

  // TODO: make this pure virtual, need to implement intersects for all rigid bodies.
  virtual bool intersects (const Ray& ray, Intersection& isect) { return false; };
};

#endif
