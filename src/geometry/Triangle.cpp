#include <iostream>
#include <algorithm>
#include <cmath>

#include "geometry/Triangle.h"
#include "helpers/RandomUtils.h"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

using namespace std;
using namespace glm;

BoundingBox Triangle::getBoundingBox () const {
  BoundingBox box;
  for (glm::vec3 point : points)
    box.add(points);
  return box;
}

bool Triangle::intersects (const Ray& ray, Intersection& isect) const {
  vec3 o = ray.position;
  vec3 v = ray.direction;

  vec3 a = this->points[0];
  vec3 b = this->points[1];
  vec3 c = this->points[2];

  vec3 n = cross(b - a, c - a);

  // Degenerate case: completely aligned.
  if (dot(n, v) == 0)
    return false;

  // Want to find intersection with infinite plane.
  // t, such that dot([o + vt] - p, n) = 0.
  // Basic derivation -
  // dot([o - p] + vt, n) = 0.
  // dot(o - p, n) + dot(vt, n) = 0.
  // dot(o - p, n) + t dot(v, n) = 0.
  // t = -dot(o - p, n) / dot(v, n).
  float x = dot(o - a, n);
  float y = dot(v, n);
  float t = -x / y;

  // Behind ray origin.
  if (t < 0.0)
    return false;

  // Now check to see point on plane lies within triangle bounds.
  vec3 z = o + v * t;

  vec3 az = a - z;
  vec3 bz = b - z;
  vec3 cz = c - z;

  bool sign1 = (dot(cross(bz, az), cross(cz, bz)) > 0);
  bool sign2 = (dot(cross(az, cz), cross(cz, bz)) > 0);

  isect.displacement = z;

  return (sign1 == sign2);
}
