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
  const vec3 &o = ray.position;
  const vec3 &v = ray.direction;

  const vec3 &a = this->points[0];
  const vec3 &b = this->points[1];
  const vec3 &c = this->points[2];

  vec3 n = cross(b - a, c - a);

  // Degenerate case: completely aligned.
  if (dot(n, v) == 0.0f)
    return (dot(a - o, n) == 0.0f);

  // Want to find intersection with infinite plane.
  // t, such that dot([o + vt] - p, n) = 0.
  // Some derivations -
  // dot([o - p] + vt, n) = 0.
  // dot(o - p, n) + dot(vt, n) = 0.
  // dot(o - p, n) + t dot(v, n) = 0.
  // t = -dot(o - p, n) / dot(v, n).
  float x = dot(o - a, n);
  float y = dot(v, n);
  float t = -x / y;

  // Behind ray origin.
  if (t < 0.0f)
    return false;

  // Now check to see point on plane lies within triangle bounds.
  vec3 z = o + v * t;

  bool sign1 = (dot(cross(z - c, a - c), cross(z - b, c - b)) > 0.0f);
  bool sign2 = (dot(cross(z - a, b - a), cross(z - b, c - b)) > 0.0f);

  // Got a hit.
  if (sign1 && sign2) {
    if (!isect.hit || (t < isect.timeHit)) {
      isect.displacement = z;
      isect.timeHit = t;
      isect.hit = true;
    }
    return true;
  }

  return false;
}
