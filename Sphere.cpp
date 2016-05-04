#include <iostream>
#include <algorithm>

#include "Sphere.h"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "RandomUtils.h"

using namespace std;

float CHEAT = 0.10;

bool Sphere::intersects (Sphere& other, Intersection& isect) {
  glm::vec3 normal = other.position - position;

  if (glm::length2(normal) > radius + other.radius + CHEAT)
    return false;

  isect.hit = true;

  normal = glm::normalize(normal);

  glm::vec3 rv = other.velocity - velocity;

  float rvNormal = glm::dot(rv, normal);

  if (rvNormal > 0.0f)
    return true;

  float e = 1.0f;

  float j = -(1.0f + e) * rvNormal / ((1.0f / mass) + (1.0f / other.mass));

  glm::vec3 impulse = j * normal;

  // velocity -= 1.0f / mass * impulse;
  // other.velocity += 1.0f / other.mass * impulse;

  isect.displacement += -1.0f / mass * impulse;

  return true;
}

bool Sphere::intersects (Plane& other, Intersection& isect) {
  glm::vec3 toSphere = position - other.position;
  float dotSN = glm::dot(toSphere, other.normal);
  glm::vec3 proj = other.normal * dotSN;

  if (glm::length2(proj) > radius * radius + CHEAT && dotSN > CHEAT)
    return false;

  isect.hit = true;

  glm::vec3 rv = other.velocity - 1.1f * velocity;

  float rvNormal = glm::dot(rv, other.normal);

  if (rvNormal < 0.0f)
    return true;

  float e = 1.0f;
  float j = -(1.0f + e) * rvNormal / ((1.0f / mass) + (1.0f / other.mass));

  glm::vec3 impulse = j * other.normal;

  // velocity -= 1.0f / mass * impulse;
  isect.displacement += -1.0f / mass * impulse;

  if (false)
    other.velocity += 1.0f / other.mass * impulse;

  // cout << velocity << endl;
  // cout << "rv: " << rv << " dv: " << -1.0f / mass * impulse << endl;

  return true;
}
