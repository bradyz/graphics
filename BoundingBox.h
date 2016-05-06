#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <algorithm>
#include <iostream>

#include <glm/glm.hpp>

#include "RandomUtils.h"

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

  void add (const glm::vec3& point) {
    isEmpty = false;
    for (int i = 0; i < 3; ++i) {
      minVals[i] = std::min(minVals[i], point[i]);
      maxVals[i] = std::max(maxVals[i], point[i]);
    }
  }

  void add (const std::vector<glm::vec3>& points) {
    for (const glm::vec3& point: points)
      add(point);
  }

  void add (const std::vector<glm::vec4>& points) {
    for (const glm::vec4& point: points)
      add(glm::vec3(point));
  }

  void merge (const BoundingBox& other) {
    if (other.isEmpty)
      return;
    isEmpty = false;
    for (int i = 0; i < 3; ++i) {
      minVals[i] = std::min(minVals[i], other.minVals[i]);
      maxVals[i] = std::max(maxVals[i], other.maxVals[i]);
    }
  }

  std::vector<glm::vec4> getVertices () const {
    std::vector<glm::vec4> vertices;

    vertices.push_back(glm::vec4(minVals.x, minVals.y, minVals.z, 1.0));
    vertices.push_back(glm::vec4(maxVals.x, minVals.y, minVals.z, 1.0));
    vertices.push_back(glm::vec4(maxVals.x, minVals.y, maxVals.z, 1.0));
    vertices.push_back(glm::vec4(minVals.x, minVals.y, maxVals.z, 1.0));

    vertices.push_back(glm::vec4(minVals.x, maxVals.y, minVals.z, 1.0));
    vertices.push_back(glm::vec4(maxVals.x, maxVals.y, minVals.z, 1.0));
    vertices.push_back(glm::vec4(maxVals.x, maxVals.y, maxVals.z, 1.0));
    vertices.push_back(glm::vec4(minVals.x, maxVals.y, maxVals.z, 1.0));

    return vertices;
  }

  std::vector<glm::uvec2> getEdges () const {
    std::vector<glm::uvec2> edges;

    edges.push_back(glm::uvec2(0, 1));
    edges.push_back(glm::uvec2(1, 2));
    edges.push_back(glm::uvec2(2, 3));
    edges.push_back(glm::uvec2(3, 0));

    edges.push_back(glm::uvec2(4, 5));
    edges.push_back(glm::uvec2(5, 6));
    edges.push_back(glm::uvec2(6, 7));
    edges.push_back(glm::uvec2(7, 4));

    edges.push_back(glm::uvec2(0, 4));
    edges.push_back(glm::uvec2(1, 5));
    edges.push_back(glm::uvec2(2, 6));
    edges.push_back(glm::uvec2(3, 7));

    return edges;
  }

  friend std::ostream& operator<< (std::ostream& os, const BoundingBox& box) {
    os << "min: " << box.minVals << " ";
    os << "max: " << box.maxVals;
    return os;
  }

  bool intersects(const BoundingBox &rhs) const {
    return 
      (rhs.minVals[0] - 1e-5 <= maxVals[0]) && (rhs.maxVals[0] + 1e-5 >= minVals[0]) &&
      (rhs.minVals[1] - 1e-5 <= maxVals[1]) && (rhs.maxVals[1] + 1e-5 >= minVals[1]) &&
      (rhs.minVals[2] - 1e-5 <= maxVals[2]) && (rhs.maxVals[2] + 1e-5 >= minVals[2]);
  }

  bool intersects(const glm::vec3& point) const {
    return 
      (point[0] + 1e-5 >= minVals[0]) && 
      (point[0] - 1e-5 <= maxVals[0]) &&
      (point[1] + 1e-5 >= minVals[1]) &&
      (point[1] - 1e-5 <= maxVals[1]) && 
      (point[2] + 1e-5 >= minVals[2]) && 
      (point[2] - 1e-5 <= maxVals[2]);
  }
};

#endif
