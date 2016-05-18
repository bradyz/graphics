#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <vector>
#include <glm/glm.hpp>

#include "BoundingBox.h"

const float DT = 0.015f;

struct RigidBody {
  glm::vec3 prev_acceleration;  
  glm::vec3 acceleration;  
  glm::vec3 velocity;  
  glm::vec3 position;

  glm::vec4 color;

  float mass;

  RigidBody () : prev_acceleration(), acceleration(), velocity(), mass(1.0) { }

  glm::vec3 stepOffset (const std::vector<glm::vec3>& forces) {
    glm::vec3 total_force;

    for (const glm::vec3& force: forces)
      total_force += force;

    acceleration = total_force / mass;
    velocity += (prev_acceleration + acceleration) / 2.0f * DT;
    velocity = 0.95f * glm::clamp(velocity, -25.0f, 25.0f);
    prev_acceleration = acceleration;

    return DT * velocity;
  } 

  virtual BoundingBox getBoundingBox () const = 0;
};

#endif
