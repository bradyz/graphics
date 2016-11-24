#include <iostream>
#include <algorithm>
#include <cmath>

#include "Sphere.h"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "RandomUtils.h"

using namespace std;

bool Sphere::intersects (const Sphere& other, Intersection& isect) const {
  glm::vec3 normal = other.position - position;

  if (glm::length(normal) > radius + other.radius)
    return false;

  isect.hit = true;

  normal = glm::normalize(normal);

  glm::vec3 rv = other.velocity - velocity;

  float rvNormal = glm::dot(rv, normal);

  if (rvNormal > 0.0f)
    return true;

  float j = -(2.0f) * rvNormal / ((1.0f / mass) + (1.0f / other.mass));
  glm::vec3 impulse = j * normal;

  isect.displacement += -1.0f / mass * impulse;

  return true;
}

bool Sphere::intersects (const Plane& other, Intersection& isect) const {
  glm::vec3 toSphere = position - other.position;
  float dotSN = glm::dot(toSphere, other.normal);
  glm::vec3 proj = other.normal * dotSN;
  float penetrate = glm::length(proj);

  if (penetrate > radius)
    return false;

  isect.hit = true;

  glm::vec3 rv = other.velocity - velocity;
  float rvNormal = glm::dot(rv, other.normal);
  float j = -0.35f * rvNormal;
  glm::vec3 impulse = j * other.normal;

  isect.displacement += -1.0f / mass * impulse;
  
  // if (penetrate > 0.01f)
  //   position += glm::length(proj) * other.normal;

  return true;
}

bool Sphere::intersects (const BoundingBox& other, Intersection& isect) const {
  glm::vec3 diff;
  for (int i = 0; i < 3; ++i) {
    if (position[i] < other.minVals[i]) 
      diff[i] += other.minVals[i] - position[i];
    if (position[i] > other.maxVals[i]) 
      diff[i] += position[i] - other.maxVals[i];
  }
  if (glm::length2(diff) < radius * radius) {
    return true;
  }
  return false;
}

bool Sphere::intersects (const Ray& ray, Intersection& isect) const {
  double a = 0.0;
  double b = 0.0;
  double c = - (radius * radius);
  for (int i = 0; i < 3; ++i) {
    a += ray.direction[i] * ray.direction[i];
    b += 2.0 * ray.direction[i] * (ray.position[i] - position[i]);
    c += (ray.position[i] - position[i]) * (ray.position[i] - position[i]);
  }
  if (b * b - 4.0 * a * c < 0.0) {
    return false;
  }
  double t1 = (-b + sqrt(b * b - 4.0 * a * c)) / (2.0 * a);
  double t2 = (-b - sqrt(b * b - 4.0 * a * c)) / (2.0 * a);
  if (t1 < 0.0 && t2 < 0.0) {
    return false;
  }
  isect.hit = true;
  if (t1 < 0.0 || t2 < 0.0)
    isect.timeHit = max(t1, t2);
  else
    isect.timeHit = min(t1, t2);
  return true;
}
