#include <iostream>

#include "Sphere.h"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "RandomUtils.h"

using namespace std;

bool Sphere::intersects (Sphere& other) {
  glm::vec3 normal = other.position - position;

  if (glm::length2(normal) > radius * radius + other.radius * other.radius)
    return false;

  normal = glm::normalize(normal);

  glm::vec3 rv = other.velocity - velocity;

  float rvNormal = glm::dot(rv, normal);

  if (rvNormal > 0.0f)
    return true;

  float e = 1.0f;

  float j = -(1.0f + e) * rvNormal / ((1.0f / mass) + (1.0f / other.mass));

  glm::vec3 impulse = j * normal;
  velocity -= 1.0f / mass * impulse;
  other.velocity += 1.0f / other.mass * impulse;

  return true;
}
