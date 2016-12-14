#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <algorithm>
#include <iostream>

#include <glm/glm.hpp>

#include "helpers/RandomUtils.h"
#include "physics/Intersection.h"
#include "geometry/Ray.h"

const double INF = 1e9;

struct BoundingBox {
  bool isEmpty;

  glm::vec3 minVals;
  glm::vec3 maxVals;

  BoundingBox (const std::vector<glm::vec4>& points=std::vector<glm::vec4>()) :
    isEmpty(points.size() > 0), minVals(INF, INF, INF), maxVals(-INF, -INF, -INF) {
    add(points);
  }

  BoundingBox (const glm::vec3& minV, const glm::vec3& maxV) :
    isEmpty(false), minVals(minV), maxVals(maxV) { }

  void add (const glm::vec3& point);
  void add (const std::vector<glm::vec3>& points);
  void add (const std::vector<glm::vec4>& points);

  void merge (const BoundingBox& other);

  float getSurfaceArea () const;

  std::vector<glm::vec4> getVertices () const;
  std::vector<glm::uvec2> getEdges () const;

  bool intersects (const BoundingBox &rhs) const;
  bool intersects (const glm::vec3& point) const;
  bool intersects (const Ray& r, Intersection& isect) const;

  friend std::ostream& operator<< (std::ostream& os, const BoundingBox& box) {
    os << "min: " << box.minVals << " ";
    os << "max: " << box.maxVals;
    return os;
  }
};

#endif
