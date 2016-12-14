#include <algorithm>
#include <iostream>

#include "geometry/BoundingBox.h"

using namespace glm;
using namespace std;

void BoundingBox::add (const vec3& point) {
  isEmpty = false;
  for (int i = 0; i < 3; ++i) {
    minVals[i] = std::min(minVals[i], point[i]);
    maxVals[i] = std::max(maxVals[i], point[i]);
  }
}

void BoundingBox::add (const vector<vec3>& points) {
  for (const vec3& point: points)
    add(point);
}

void BoundingBox::add (const vector<vec4>& points) {
  for (const vec4& point: points)
    add(vec3(point));
}

void BoundingBox::merge (const BoundingBox& other) {
  if (other.isEmpty)
    return;
  isEmpty = false;
  for (int i = 0; i < 3; ++i) {
    minVals[i] = std::min(minVals[i], other.minVals[i]);
    maxVals[i] = std::max(maxVals[i], other.maxVals[i]);
  }
}

float BoundingBox::getSurfaceArea () const {
  if (isEmpty)
    return 0.0f;
  float sa = 0.0f;
  sa += 2.0f * (maxVals[0] - minVals[0]) * (maxVals[1] - minVals[1]);
  sa += 2.0f * (maxVals[0] - minVals[0]) * (maxVals[2] - minVals[2]);
  sa += 2.0f * (maxVals[1] - minVals[1]) * (maxVals[2] - minVals[2]);
  return sa;
}

vector<vec4> BoundingBox::getVertices () const {
  vector<vec4> vertices;

  vertices.push_back(vec4(minVals.x, minVals.y, minVals.z, 1.0));
  vertices.push_back(vec4(maxVals.x, minVals.y, minVals.z, 1.0));
  vertices.push_back(vec4(maxVals.x, minVals.y, maxVals.z, 1.0));
  vertices.push_back(vec4(minVals.x, minVals.y, maxVals.z, 1.0));

  vertices.push_back(vec4(minVals.x, maxVals.y, minVals.z, 1.0));
  vertices.push_back(vec4(maxVals.x, maxVals.y, minVals.z, 1.0));
  vertices.push_back(vec4(maxVals.x, maxVals.y, maxVals.z, 1.0));
  vertices.push_back(vec4(minVals.x, maxVals.y, maxVals.z, 1.0));

  return vertices;
}

vector<uvec2> BoundingBox::getEdges () const {
  vector<uvec2> edges;

  edges.push_back(uvec2(0, 1));
  edges.push_back(uvec2(1, 2));
  edges.push_back(uvec2(2, 3));
  edges.push_back(uvec2(3, 0));

  edges.push_back(uvec2(4, 5));
  edges.push_back(uvec2(5, 6));
  edges.push_back(uvec2(6, 7));
  edges.push_back(uvec2(7, 4));

  edges.push_back(uvec2(0, 4));
  edges.push_back(uvec2(1, 5));
  edges.push_back(uvec2(2, 6));
  edges.push_back(uvec2(3, 7));

  return edges;
}

bool BoundingBox::intersects (const BoundingBox &rhs) const {
  return 
    (rhs.minVals[0] - 1e-5 <= maxVals[0]) && (rhs.maxVals[0] + 1e-5 >= minVals[0]) &&
    (rhs.minVals[1] - 1e-5 <= maxVals[1]) && (rhs.maxVals[1] + 1e-5 >= minVals[1]) &&
    (rhs.minVals[2] - 1e-5 <= maxVals[2]) && (rhs.maxVals[2] + 1e-5 >= minVals[2]);
}

bool BoundingBox::intersects (const vec3& point) const {
  return 
    (point[0] + 1e-5 >= minVals[0]) && 
    (point[0] - 1e-5 <= maxVals[0]) &&
    (point[1] + 1e-5 >= minVals[1]) &&
    (point[1] - 1e-5 <= maxVals[1]) && 
    (point[2] + 1e-5 >= minVals[2]) && 
    (point[2] - 1e-5 <= maxVals[2]);
}

bool BoundingBox::intersects (const Ray& r, Intersection& isect) const {
  vec3 R0 = r.position;
  vec3 Rd = r.direction;
  double tMin = -1.0e308;
  double tMax = 1.0e308;
  for (int currentaxis = 0; currentaxis < 3; currentaxis++) {
    double vd = Rd[currentaxis];
    if (vd == 0.0)
      continue;
    double v1 = minVals[currentaxis] - R0[currentaxis];
    double v2 = maxVals[currentaxis] - R0[currentaxis];
    // two slab intersections
    double t1 = v1 / vd;
    double t2 = v2 / vd;
    if (t1 > t2) {
      double ttemp = t1;
      t1 = t2;
      t2 = ttemp;
    }
    if (t1 > tMin) tMin = t1;
    if (t2 < tMax) tMax = t2;
    cout << tMin << " " << tMax << endl;
    if (tMin > tMax) return false; // box is missed
    if (tMax < 1e-5) return false; // box is behind ray
  }
  cout << "sucess" << endl;
  isect.timeHit = tMin;
  return true; // it made it past all 3 axes.
}
